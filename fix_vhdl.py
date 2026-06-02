import sys

f = r'D:\0_PRJ\Git\data_sampling\FPGA\srcs\sources_1\ScopeFun_core.vhd'

with open(f, 'r', encoding='utf-8') as fh:
    lines = fh.readlines()

print('File has', len(lines), 'lines')

# 1. Add constants after 'CONSTANT bL'
for i,l in enumerate(lines):
    if 'CONSTANT bL' in l:
        indent = l[:len(l)-len(l.lstrip())]
        extra = ['\n',
                 indent + '-- TEST_FRAME_MODE: direct EP6 test, no ADC needed\n',
                 indent + 'CONSTANT TEST_FRAME_MODE  : boolean  := True;\n',
                 indent + 'CONSTANT TEST_FRAME_WORDS : integer := 256;\n']
        for line in reversed(extra): lines.insert(i+1, line)
        print('1. Constants added at line', i)
        break

# 2. Add test counter signal
for i,l in enumerate(lines):
    if 'signal pktend_i' in l:
        indent = l[:len(l)-len(l.lstrip())]
        extra = ['\n',
                 indent + '-- test frame mode counter\n',
                 indent + "signal test_frame_cnt : unsigned(26 downto 0) := (others => '0');\n"]
        for line in reversed(extra): lines.insert(i+1, line)
        print('2. Signal added at line', i)
        break

# 3. Insert at top of state B
for i,l in enumerate(lines):
    if 'when B =>' in l and i > 1300:
        indent = lines[i][:len(lines[i])-len(lines[i].lstrip())]
        ins = [indent + "if TEST_FRAME_MODE and flaga_d = '1' then\n",
               indent + "    faddr_i <= \"00\";\n",
               indent + "    hword_cnt_i <= 1;\n",
               indent + "    slwr_assert_cnt <= 1;\n",
               indent + "    test_frame_cnt <= (others => '0');\n",
               indent + "    Masterstate <= G;\n",
               indent + "end if;\n"]
        for line in reversed(ins): lines.insert(i+1, line)
        print('3. State B override at line', i)
        break

# 4. Add test data in state G sample section
for i,l in enumerate(lines):
    if l.strip() == "SendingFrameSlow <= '0';" and i > 1700:
        print('4. Found SendingFrameSlow at line', i)
        for j in range(i, i-10, -1):
            if 'if ( send_sample_cnt = to_integer' in lines[j]:
                indent_if = lines[j][:len(lines[j])-len(lines[j].lstrip())]
                depth = 1
                for k in range(j+1, min(j+40, len(lines))):
                    stripped = lines[k].strip()
                    if stripped.startswith('if ') or stripped.startswith('elsif '):
                        depth += 1
                    if stripped == 'end if;':
                        depth -= 1
                        if depth == 0:
                            break
                    if stripped == 'else' and depth == 1:
                        indent_else = lines[k][:len(lines[k])-len(lines[k].lstrip())]
                        ins = [indent_else + "elsif TEST_FRAME_MODE then\n",
                               indent_else + "    fdata <= std_logic_vector(test_frame_cnt) & \"00000\";\n",
                               indent_else + "    test_frame_cnt <= test_frame_cnt + 1;\n",
                               indent_else + "    pktend_i <= '1';\n",
                               indent_else + "    DataOutEnable <= '0';\n",
                               indent_else + "    send_sample_cnt <= send_sample_cnt + 1;\n",
                               indent_else + "    Masterstate <= G;\n",
                               indent_else + "else\n"]
                        lines.pop(k)
                        for line in reversed(ins): lines.insert(k, line)
                        print('   Test data added at line', k)
                        break
                break
        break

# 5. Reduce init timer
for i,l in enumerate(lines):
    if 'Timer_cnt = 50000' in l and i > 1300:
        lines[i] = lines[i].replace('Timer_cnt = 50000', 'Timer_cnt = 500')
        print('5. Timer reduced at line', i)
        break

with open(f, 'w', encoding='utf-8') as fh:
    fh.writelines(lines)
print('ALL DONE')
