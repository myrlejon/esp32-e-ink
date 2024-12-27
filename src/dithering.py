from PIL import Image
import numpy as np

img = Image.open("../img/frame.png").convert("L")
img = img.resize((122, 250), Image.Resampling.LANCZOS)
img = img.convert("1")

img.save("image.bmp")

img_array = np.array(img)
byte_array = bytearray()
width, height = img.size

for y in range(height):
    byte = 0
    for x in range(width):
        pixel = img_array[y, x]  # get the pixel value (0 or 255)
        if pixel == 0:  # black pixel
            byte |= (1 << (7 - (x % 8)))
        if (x % 8) == 7: 
            byte_array.append(byte)
            byte = 0
    if width % 8 != 0:  # pad the remaining bits in the last byte
        byte_array.append(byte)


print(byte_array)
file = open("image.h", "w")

file.write("unsigned char image_array[] = {" + f"\n")

counter = 1
for x in byte_array:
    file.write(f"0x{x:02X}, ")
    if (counter % 10 == 0):
        file.write("\n")
    counter = counter + 1
file.write("};")
file.close()