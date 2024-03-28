s = '''
    // 0 hv1
    {8, {COORD_A1, COORD_B1, COORD_C1, COORD_D1, COORD_E1, COORD_F1, COORD_G1, COORD_H1}},
    {8, {COORD_H1, COORD_H2, COORD_H3, COORD_H4, COORD_H5, COORD_H6, COORD_H7, COORD_H8}},
    {8, {COORD_H8, COORD_G8, COORD_F8, COORD_E8, COORD_D8, COORD_C8, COORD_B8, COORD_A8}},
    {8, {COORD_A8, COORD_A7, COORD_A6, COORD_A5, COORD_A4, COORD_A3, COORD_A2, COORD_A1}},

    // 1 hv2
    {8, {COORD_A2, COORD_B2, COORD_C2, COORD_D2, COORD_E2, COORD_F2, COORD_G2, COORD_H2}},
    {8, {COORD_B1, COORD_B2, COORD_B3, COORD_B4, COORD_B5, COORD_B6, COORD_B7, COORD_B8}},
    {8, {COORD_A7, COORD_B7, COORD_C7, COORD_D7, COORD_E7, COORD_F7, COORD_G7, COORD_H7}},
    {8, {COORD_G1, COORD_G2, COORD_G3, COORD_G4, COORD_G5, COORD_G6, COORD_G7, COORD_G8}},

    // 2 hv3
    {8, {COORD_A3, COORD_B3, COORD_C3, COORD_D3, COORD_E3, COORD_F3, COORD_G3, COORD_H3}},
    {8, {COORD_C1, COORD_C2, COORD_C3, COORD_C4, COORD_C5, COORD_C6, COORD_C7, COORD_C8}},
    {8, {COORD_A6, COORD_B6, COORD_C6, COORD_D6, COORD_E6, COORD_F6, COORD_G6, COORD_H6}},
    {8, {COORD_F1, COORD_F2, COORD_F3, COORD_F4, COORD_F5, COORD_F6, COORD_F7, COORD_F8}},

    // 3 hv4
    {8, {COORD_A4, COORD_B4, COORD_C4, COORD_D4, COORD_E4, COORD_F4, COORD_G4, COORD_H4}},
    {8, {COORD_D1, COORD_D2, COORD_D3, COORD_D4, COORD_D5, COORD_D6, COORD_D7, COORD_D8}},
    {8, {COORD_A5, COORD_B5, COORD_C5, COORD_D5, COORD_E5, COORD_F5, COORD_G5, COORD_H5}},
    {8, {COORD_E1, COORD_E2, COORD_E3, COORD_E4, COORD_E5, COORD_E6, COORD_E7, COORD_E8}},

    // 4 d5 + corner + X
    {7, {COORD_D1, COORD_E2, COORD_F3, COORD_G4, COORD_H5, COORD_H1, COORD_G2, COORD_NO}},
    {7, {COORD_E1, COORD_D2, COORD_C3, COORD_B4, COORD_A5, COORD_A1, COORD_B2, COORD_NO}},
    {7, {COORD_A4, COORD_B5, COORD_C6, COORD_D7, COORD_E8, COORD_A8, COORD_B7, COORD_NO}},
    {7, {COORD_H4, COORD_G5, COORD_F6, COORD_E7, COORD_D8, COORD_H8, COORD_G7, COORD_NO}},

    // 5 d6
    {6, {COORD_C1, COORD_D2, COORD_E3, COORD_F4, COORD_G5, COORD_H6, COORD_NO, COORD_NO}},
    {6, {COORD_F1, COORD_E2, COORD_D3, COORD_C4, COORD_B5, COORD_A6, COORD_NO, COORD_NO}},
    {6, {COORD_A3, COORD_B4, COORD_C5, COORD_D6, COORD_E7, COORD_F8, COORD_NO, COORD_NO}},
    {6, {COORD_H3, COORD_G4, COORD_F5, COORD_E6, COORD_D7, COORD_C8, COORD_NO, COORD_NO}},

    // 6 d7
    {7, {COORD_B1, COORD_C2, COORD_D3, COORD_E4, COORD_F5, COORD_G6, COORD_H7, COORD_NO}},
    {7, {COORD_G1, COORD_F2, COORD_E3, COORD_D4, COORD_C5, COORD_B6, COORD_A7, COORD_NO}},
    {7, {COORD_A2, COORD_B3, COORD_C4, COORD_D5, COORD_E6, COORD_F7, COORD_G8, COORD_NO}},
    {7, {COORD_H2, COORD_G3, COORD_F4, COORD_E5, COORD_D6, COORD_C7, COORD_B8, COORD_NO}},

    // 7 d8
    {8, {COORD_A1, COORD_B2, COORD_C3, COORD_D4, COORD_E5, COORD_F6, COORD_G7, COORD_H8}},
    {8, {COORD_H1, COORD_G2, COORD_F3, COORD_E4, COORD_D5, COORD_C6, COORD_B7, COORD_A8}},

    // dummy
    {0, {COORD_NO, COORD_NO, COORD_NO, COORD_NO, COORD_NO, COORD_NO, COORD_NO, COORD_NO}},
    {0, {COORD_NO, COORD_NO, COORD_NO, COORD_NO, COORD_NO, COORD_NO, COORD_NO, COORD_NO}},

    // 8 corner-edge + 2x
    {8, {COORD_B2, COORD_A1, COORD_B1, COORD_C1, COORD_F1, COORD_G1, COORD_H1, COORD_G2}},
    {8, {COORD_B2, COORD_A1, COORD_A2, COORD_A3, COORD_A6, COORD_A7, COORD_A8, COORD_B7}},
    {8, {COORD_B7, COORD_A8, COORD_B8, COORD_C8, COORD_F8, COORD_G8, COORD_H8, COORD_G7}},
    {8, {COORD_G2, COORD_H1, COORD_H2, COORD_H3, COORD_H6, COORD_H7, COORD_H8, COORD_G7}},

    // 9 small triangle
    {8, {COORD_A1, COORD_B1, COORD_C1, COORD_D1, COORD_A2, COORD_B2, COORD_A3, COORD_A4}},
    {8, {COORD_H1, COORD_G1, COORD_F1, COORD_E1, COORD_H2, COORD_G2, COORD_H3, COORD_H4}},
    {8, {COORD_A8, COORD_B8, COORD_C8, COORD_D8, COORD_A7, COORD_B7, COORD_A6, COORD_A5}},
    {8, {COORD_H8, COORD_G8, COORD_F8, COORD_E8, COORD_H7, COORD_G7, COORD_H6, COORD_H5}},

    // 10 corner + small-block
    {8, {COORD_A1, COORD_C1, COORD_D1, COORD_E1, COORD_F1, COORD_H1, COORD_C2, COORD_F2}},
    {8, {COORD_A1, COORD_A3, COORD_A4, COORD_A5, COORD_A6, COORD_A8, COORD_B3, COORD_B6}},
    {8, {COORD_A8, COORD_C8, COORD_D8, COORD_E8, COORD_F8, COORD_H8, COORD_C7, COORD_F7}},
    {8, {COORD_H1, COORD_H3, COORD_H4, COORD_H5, COORD_H6, COORD_H8, COORD_G3, COORD_G6}},

    // 11 corner8
    {8, {COORD_A1, COORD_B1, COORD_C1, COORD_A2, COORD_B2, COORD_C2, COORD_A3, COORD_B3}},
    {8, {COORD_H1, COORD_G1, COORD_F1, COORD_H2, COORD_G2, COORD_F2, COORD_H3, COORD_G3}},
    {8, {COORD_A8, COORD_B8, COORD_C8, COORD_A7, COORD_B7, COORD_C7, COORD_A6, COORD_B6}},
    {8, {COORD_H8, COORD_G8, COORD_F8, COORD_H7, COORD_G7, COORD_F7, COORD_H6, COORD_G6}},

    // 12 corner-stability + 2 corner
    {8, {COORD_A1, COORD_A6, COORD_A7, COORD_A8, COORD_B7, COORD_B8, COORD_C8, COORD_H8}},
    {8, {COORD_H8, COORD_H3, COORD_H2, COORD_H1, COORD_G2, COORD_G1, COORD_F1, COORD_A1}},
    {8, {COORD_H1, COORD_H6, COORD_H7, COORD_H8, COORD_G7, COORD_G8, COORD_F8, COORD_A8}},
    {8, {COORD_A8, COORD_A3, COORD_A2, COORD_A1, COORD_B2, COORD_B1, COORD_C1, COORD_H1}},

    // 13 half center-block
    {8, {COORD_C3, COORD_D3, COORD_C4, COORD_D4, COORD_D5, COORD_C5, COORD_D6, COORD_C6}},
    {8, {COORD_F6, COORD_E6, COORD_F5, COORD_E5, COORD_E4, COORD_F4, COORD_E3, COORD_F3}},
    {8, {COORD_C3, COORD_C4, COORD_D3, COORD_D4, COORD_E4, COORD_E3, COORD_F4, COORD_F3}},
    {8, {COORD_F6, COORD_F5, COORD_E6, COORD_E5, COORD_D5, COORD_D6, COORD_C5, COORD_C6}},

    // 14 hamburger
    {8, {COORD_C2, COORD_D1, COORD_D2, COORD_D3, COORD_E3, COORD_E2, COORD_E1, COORD_F2}},
    {8, {COORD_G3, COORD_H4, COORD_G4, COORD_F4, COORD_F5, COORD_G5, COORD_H5, COORD_G6}},
    {8, {COORD_F7, COORD_E8, COORD_E7, COORD_E6, COORD_D6, COORD_D7, COORD_D8, COORD_C7}},
    {8, {COORD_B6, COORD_A5, COORD_B5, COORD_C5, COORD_C4, COORD_B4, COORD_A4, COORD_B3}},

    // 15 mid block
    {8, {COORD_C2, COORD_C3, COORD_D2, COORD_D3, COORD_E3, COORD_E2, COORD_F3, COORD_F2}},
    {8, {COORD_G3, COORD_F3, COORD_G4, COORD_F4, COORD_F5, COORD_G5, COORD_F6, COORD_G6}},
    {8, {COORD_F7, COORD_F6, COORD_E7, COORD_E6, COORD_D6, COORD_D7, COORD_C6, COORD_C7}},
    {8, {COORD_B6, COORD_C6, COORD_B5, COORD_C5, COORD_C4, COORD_B4, COORD_C3, COORD_B3}},

    // 16 I
    {8, {COORD_D1, COORD_D2, COORD_D3, COORD_D4, COORD_E4, COORD_E3, COORD_E2, COORD_E1}},
    {8, {COORD_H4, COORD_G4, COORD_F4, COORD_E4, COORD_E5, COORD_F5, COORD_G5, COORD_H5}},
    {8, {COORD_E8, COORD_E7, COORD_E6, COORD_E5, COORD_D5, COORD_D6, COORD_D7, COORD_D8}},
    {8, {COORD_A5, COORD_B5, COORD_C5, COORD_D5, COORD_D4, COORD_C4, COORD_B4, COORD_A4}},

    // 17 4x2-A
    {8, {COORD_A1, COORD_B1, COORD_A2, COORD_B2, COORD_C3, COORD_D3, COORD_C4, COORD_D4}},
    {8, {COORD_H1, COORD_H2, COORD_G1, COORD_G2, COORD_F3, COORD_F4, COORD_E3, COORD_E4}},
    {8, {COORD_H8, COORD_G8, COORD_H7, COORD_G7, COORD_F6, COORD_E6, COORD_F5, COORD_E5}},
    {8, {COORD_A8, COORD_A7, COORD_B8, COORD_B7, COORD_C6, COORD_C5, COORD_D6, COORD_D5}},

    // 18 4x2-B
    {8, {COORD_C1, COORD_D1, COORD_C2, COORD_D2, COORD_A3, COORD_B3, COORD_A4, COORD_B4}},
    {8, {COORD_H3, COORD_H4, COORD_G3, COORD_G4, COORD_F1, COORD_F2, COORD_E1, COORD_E2}},
    {8, {COORD_F8, COORD_E8, COORD_F7, COORD_E7, COORD_H6, COORD_G6, COORD_H5, COORD_G5}},
    {8, {COORD_A6, COORD_A5, COORD_B6, COORD_B5, COORD_C8, COORD_C7, COORD_D8, COORD_D7}},

    // 19 4x2-C
    {8, {COORD_A1, COORD_B1, COORD_A2, COORD_B2, COORD_B7, COORD_A7, COORD_B8, COORD_A8}},
    {8, {COORD_H1, COORD_H2, COORD_G1, COORD_G2, COORD_B2, COORD_B1, COORD_A2, COORD_A1}},
    {8, {COORD_H8, COORD_G8, COORD_H7, COORD_G7, COORD_G2, COORD_H2, COORD_G1, COORD_H1}},
    {8, {COORD_A8, COORD_A7, COORD_B8, COORD_B7, COORD_G7, COORD_G8, COORD_H7, COORD_H8}}
'''

ss = s.splitlines()
s = ''
for sss in ss:
    sss = sss.replace('    ', '')
    if len(sss):
        if sss[0] == '{':
            s += sss + '\n'

s = s.replace('{10, {', '').replace('{9, {', '').replace('{8, {', '').replace('{8,  {', '').replace('{7,  {', '').replace('{7, {', '').replace('{6, {', '').replace('{5, {', '').replace('{4, {', '').replace('{0, {', '').replace('\n\n', '\n').replace('}', '').replace('    ', '')
for num in reversed(range(100)):
    s = s.replace(', // ' + str(num), '')
    s = s.replace('  // ' + str(num), '')
    ss = '// ' + str(num) + ' '
    idx = s.find(ss)
    if idx >= 0:
        end = idx
        for i in range(idx, len(s)):
            if s[i] == '\n':
                end = i
                break
        s = s.replace(s[idx:end + 1], '')

s = s.replace(',', '')
s = s.replace('COORD_NO', '')

print(s)

ss = [line.split() for line in s.splitlines()]

def cell_to_coord(cell):
    cell = 63 - cell
    x = cell % 8
    y = cell // 8
    return 'COORD_' + chr(ord('A') + x) + str(y + 1)

def digit_space(n, r):
    n = str(n)
    l = len(n)
    for i in range(r - l):
        n = ' ' + n
    return n

#for cell in range(64):
#    print('#define COORD_' + cell_to_coord(cell) + ' ' + str(cell))

res = ''
for cell in range(64):
    coord = cell_to_coord(cell)
    tmp_arr = []
    for i in range(len(ss)):
        if coord in ss[i]:
            ii = i
            #if i >= 26:
            #    ii += 2
            tmp = '{' + digit_space(ii, 2) + ', P3' + str(len(ss[i]) - 1 - ss[i].index(coord)) + '}'
            tmp_arr.append(tmp)
    len_main = len(tmp_arr)
    for i in range(16 - len_main):
        tmp = '{ 0, PNO}'
        tmp_arr.append(tmp)
    res_tmp = '{' + digit_space(len_main, 2) + ', {' + ', '.join(tmp_arr) + '}}, // ' + coord
    res += res_tmp + '\n'
print(res)
