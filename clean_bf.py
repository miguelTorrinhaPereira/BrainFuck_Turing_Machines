PAIRS = {"<": "<>", ">": "<>", "+": "+-", "-": "+-", "[": "[]", "]": "[]"}


raw_bf = input()
final_bf = ""

curr_op_pair = ""
curr_count = 0

while True:
    for c in raw_bf:
        if c == " ":
            continue

        if c not in curr_op_pair:
            if curr_op_pair != "":
                sym = curr_op_pair[curr_count < 0]
                count = abs(curr_count)
                final_bf += sym * count

            curr_op_pair = PAIRS[c]
            curr_count = 0

        curr_count += 1 - 2 * curr_op_pair.index(c)

    # write last character
    if curr_op_pair != "":
        sym = curr_op_pair[curr_count < 0]
        count = abs(curr_count)
        final_bf += sym * count

    if len(final_bf) < len(raw_bf):
        raw_bf = final_bf
        final_bf = ""
        curr_op_pair = ""
        curr_count = 0
    else:
        break

print(final_bf)
