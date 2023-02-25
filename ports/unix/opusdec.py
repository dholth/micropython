import oggz

music = open("ehren-paper_lights-96.opus", "rb")

op = oggz.oggz(music)
buffer = memoryview(bytearray(23040))
output = open("decoded.raw", "w+b")

min_samples = 10000000
max_samples = 0

# print("read a packet, will it segfault?")
# for i in range(10):
#     print(op.read(512))

print("ready to decode, let's segfault!")
while True:
    content_type, samples_read, bytes_read, buflen = op.decode_opus(buffer)
    print(f"{samples_read} samples, {bytes_read} bytes, {buflen} buffer length, type: {content_type}")
    # min_samples = min(samples, min_samples)
    # max_samples = max(samples, max_samples)
    if samples_read > 0:
        output.write(buffer[:samples_read*4])

    if bytes_read == 0:
        break

print(f"{min_samples}, {max_samples} minimum and maximum samples decoded per call")
