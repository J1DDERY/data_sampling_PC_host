#!/usr/bin/env python3.10

# tested in python 3.10.12
from scopefun import *
from os import path
import numpy as np
import time
import matplotlib.pyplot as plt

nSamples = 10000
nFrames = 3

if nSamples % 4 != 0:
	print("Err0: Number of samples must be divisible by 4")
	quit()

# calculate how much data is required for selected number of Samples
szHeader = 1024
szUsbPacket = 1024
szData = szHeader * (-(-nSamples*4//szUsbPacket)) # round up
szframe = szHeader + szData

fpga_image_file  = path.relpath('../data/firmware/fpga2.bin')

# ---------------------------------------------------
# init API
# ---------------------------------------------------

sfApiInit()                         #USB context
sfctx = sfCreateSFContext()         #SFContext
sfApiCreateContext(sfctx, szframe)  #API buffer alloc
#sfSetTimeOut(sfctx, 10000)
#sfSetThreadSafe(sfctx,1)

# ---------------------------------------------------
# activate API
# ---------------------------------------------------

if sfIsActive(sfctx) == 0:
	sfSetActive(sfctx, 1)
	if sfIsActive(sfctx) == 1:
		print("ScopeFun API activated")
else:
	print("ScopeFun API is active")
print("API version: {}.{}.{}".format(sfctx.api.version, sfctx.api.major, sfctx.api.minor))

# ---------------------------------------------------
# open USB connection
# ---------------------------------------------------

susb = sfCreateSUsb()
susb.idVendor = 7504
susb.idProduct = 24836
print("Checking USB connection...", end = " ")
ret = sfHardwareOpen(sfctx, susb, 2)
if ret == 0:
	print("connected")
	# read FX3 firmware ID
	SEeprom = sfCreateSEeprom()
	ret = sfHardwareEepromReadFirmwareID(sfctx, SEeprom, 0)
	if ret == 0:
		np_eeprom_ID_bytes = np.array(SEeprom.data.bytes[0:14], copy=False)
		print("FX3 firmware ID:", np_eeprom_ID_bytes.tobytes())
	else:
		print("Reading FX3 firmware ID failed!")
else:
	print("not connected")
	print("Err0: Could not open USB connection!")
	quit()

# ---------------------------------------------------
# configure FPGA
# ---------------------------------------------------

print("Checking FPGA status...", end =" ")
# allocate SInt variable to hold FPGA status
status = sfCreateSInt()
ret = sfHardwareReadFpgaStatus(sfctx, status)
if status.value == 0:
	print("not configured")
	# create SFpga struct to hold FPGA image data and image size
	SFpga = sfCreateSFpga()
	# load FPGA image from file to numpy array
	fpga_image = np.fromfile(fpga_image_file, dtype=np.uint8)
	# set FPGA image size
	SFpga.size = fpga_image.size
	print ("Uploading FPGA firmware image (image file '{}', {} Bytes)"
		   .format(fpga_image_file, fpga_image.size))
	# numpy array sizes must be the same before copy operation
	append_len = SFpga.data.bytes.size - fpga_image.shape[0]
	fpga_image = np.append(fpga_image, np.zeros(append_len, dtype=np.uint8))
	np.copyto(SFpga.data.bytes, fpga_image)
	# array "SFpga.data.bytes" now holds FPGA image data
	del fpga_image
	# upload FPGA firmware image
	sfHardwareUploadFpga(sfctx, SFpga)
	time.sleep(1)
	ret = sfHardwareReadFpgaStatus(sfctx, status)
	if status.value == 0:
		print("Err0: FPGA configuration failed")
		quit()
	else:
		print("FPGA configuration successful")
else:
	print("configured")

# ---------------------------------------------------
# Configure hardware
# ---------------------------------------------------

print("Configuring hardware control registers...", end =" ")
SHw = sfCreateSHardware()
# set default settings
sfSetDefault(SHw)
# set number of samples
sfSetNumSamples(SHw, nSamples)
sfSetDataEncodingFormat(SHw, 0) # read data from all channels (default)
# enable AWG1
sfSetGeneratorType0(SHw, 1)                       # sin
sfSetGeneratorFrequency0(SHw, 100000, 200000000)  # 100 kHz
sfSetGeneratorVoltage0(SHw, 1500)                 # 1.5 V
sfSetGeneratorOn0(SHw, 1)
# apply SHw settings to hardware (USB transfer)
ret = sfHardwareConfig(sfctx, SHw)
if ret == 0:
	print("OK")
else:
	print("failed")

# ---------------------------------------------------
# Read frame (header + samples)
# ---------------------------------------------------

nReceived  = sfCreateSInt()
nFrameSize = sfCreateSInt()
SFFHeader = SFrameHeader()        # to hold frame header
SHwReceived = sfCreateSHardware() # to hold control registers from received header
# capture frame (header + samples)
print("Capturing {} frame(s), {} samples...".format(nFrames, nSamples))
for i in range(nFrames):
	print("Frame {}:".format(i+1))
	# capture data
	ret = sfFrameCapture(sfctx, nReceived, nFrameSize)
	# get size values for the current frame
	frameSizeBytes = nFrameSize.value           # frame size
	receivedBytes  = nReceived.value            # received bytes after calling sfFrameCapture()
	totalReceivedBytes = sfctx.frame.received   # total received bytes (will be zero if full frame was received)
	# copy frame header to SFFHeader struct
	sfGetHeader(sfctx, sfctx.frame.data, SFFHeader)
	# read control registers from header
	sfGetHeaderHardware(SFFHeader, SHwReceived)
	# get number of _received_ samples from header (this should be the same as _requested_ number of samples)
	# however, if number of samples was changed while capturing,
	# we would need to read one additional frame to get the requested new number of samples
	nSamplesInFrame = sfGetNumSamples(SHwReceived)
	# print("sfGetDataEncodingFormat: {}".format(sfGetDataEncodingFormat(SHwReceived)))
	if ret == 0:
		print("USB transfer OK: {} Bytes received".format(receivedBytes))
		# if full frame was not received
		while totalReceivedBytes > 0:
			# get more data
			ret = sfFrameCapture(sfctx, nReceived, nFrameSize)
			if ret == 0:
				totalReceivedBytes = sfctx.frame.received
				if totalReceivedBytes > 0:
					print("USB transfer OK, {} Bytes received total".format(totalReceivedBytes))
				else:
					print("USB transfer OK, {} Bytes received total".format(frameSizeBytes))
		print("{} samples received".format(nSamplesInFrame))
	else:
		print("USB transfer failed (ret {}), {} Bytes received".format(ret, receivedBytes))

# ---------------------------------------------------
# Decode samples
# ---------------------------------------------------

# read raw data from captured frame
print("Decoding received frame data...", end = " ")
frameData = np.array(sfctx.frame.data.data.bytes[szHeader:frameSizeBytes], copy=False)
print("({} bytes)".format(frameData.size))
# decode raw data
ch1 = sfGetSamplesFromBytes(1, SHwReceived, frameData)
print("Ch1:", ch1)
if ch1.size == 0:
	print("Err0: ch1 decoding error, no data provided")
ch2 = sfGetSamplesFromBytes(2, SHwReceived, frameData)
print("Ch2:", ch2)
if ch2.size == 0:
	print("Err0: ch2 decoding error, no data provided")
dig = sfGetSamplesFromBytes(3, SHwReceived, frameData)
print("Dig:", dig)
if dig.size == 0:
	print("Err0: dig decoding error, no data provided")

# ---------------------------------------------------
# Plot samples
# ---------------------------------------------------

x = np.arange(nSamplesInFrame)
# plotting
plt.title("ScopeFun")
plt.xlabel("Samples")
plt.ylabel("Values")
if ch1.size > 0:
	plt.plot(x, ch1, color ="green")
if ch2.size > 0:
	plt.plot(x, ch2, color ="blue")
print("Plotting data...")
plt.show()
plt.close()
