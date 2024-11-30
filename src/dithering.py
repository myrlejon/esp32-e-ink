from PIL import Image

img = Image.open("../img/gnawer.png").convert("L")
img = img.resize((122, 250), Image.Resampling.LANCZOS)
img = img.convert("1")
img.save("dithered_image.bmp")