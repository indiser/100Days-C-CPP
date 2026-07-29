from PIL import Image, ImageDraw

width, height = 500, 450
img = Image.new("RGB", (width, height), color=(255, 240, 245))  # soft pink bg
draw = ImageDraw.Draw(img)

black = (40, 30, 40)
white = (255, 255, 255)
fur = (255, 236, 214)       # cream fur
fur_shade = (250, 220, 190)
pink = (255, 179, 198)
blush = (255, 160, 180)
eye_color = (90, 60, 110)

cx, cy = 250, 230

# Ears
draw.polygon([(150, 130), (185, 40), (225, 120)], fill=fur, outline=black, width=3)
draw.polygon([(350, 130), (315, 40), (275, 120)], fill=fur, outline=black, width=3)
draw.polygon([(160, 120), (185, 65), (212, 112)], fill=pink)
draw.polygon([(340, 120), (315, 65), (288, 112)], fill=pink)

# Head
draw.ellipse([120, 100, 380, 340], fill=fur, outline=black, width=3)

# Cheeks (blush)
draw.ellipse([140, 240, 200, 280], fill=blush)
draw.ellipse([300, 240, 360, 280], fill=blush)

# Big anime eyes (white base)
draw.ellipse([170, 190, 235, 265], fill=white, outline=black, width=3)
draw.ellipse([265, 190, 330, 265], fill=white, outline=black, width=3)

# Iris
draw.ellipse([180, 205, 222, 252], fill=eye_color)
draw.ellipse([278, 205, 320, 252], fill=eye_color)

# Pupils
draw.ellipse([192, 218, 210, 240], fill=black)
draw.ellipse([290, 218, 308, 240], fill=black)

# Eye sparkle highlights
draw.ellipse([196, 210, 205, 219], fill=white)
draw.ellipse([294, 210, 303, 219], fill=white)
draw.ellipse([210, 235, 216, 241], fill=white)
draw.ellipse([308, 235, 314, 241], fill=white)

# Nose
draw.polygon([(240, 275), (260, 275), (250, 288)], fill=pink, outline=black, width=2)

# Mouth (small w shape)
draw.arc([225, 280, 250, 300], 0, 160, fill=black, width=3)
draw.arc([250, 280, 275, 300], 20, 180, fill=black, width=3)

# Whiskers
for y in (255, 268, 281):
    draw.line([(130, y), (60, y - 8)], fill=black, width=2)
    draw.line([(370, y), (440, y - 8)], fill=black, width=2)

img.save("cute_cat.bmp")
print("saved")