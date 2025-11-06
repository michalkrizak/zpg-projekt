# -*- coding: utf-8 -*-
"""
Skript pro vytvoření testovacích textur pro zpg-projekt
Vytváří jednoduché procedurální textury, které lze použít pro testování
"""

from PIL import Image, ImageDraw
import random

def create_formula_red_texture(size=512):
    """Vytvoří červenou texturu s jemným šumem pro formuli"""
    img = Image.new('RGB', (size, size))
    pixels = img.load()
    
    for y in range(size):
        for x in range(size):
            # Červená barva s malými náhodnými odchylkami
            noise = random.randint(-15, 15)
            r = min(255, max(0, 220 + noise))
            g = min(255, max(0, 30 + noise))
            b = min(255, max(0, 30 + noise))
            pixels[x, y] = (r, g, b)
    
    return img

def create_wood_texture(size=512):
    """Vytvoří dřevěnou texturu s pruhy"""
    img = Image.new('RGB', (size, size))
    pixels = img.load()
    
    for y in range(size):
        for x in range(size):
            # Dřevěné pruhy
            value = int((x + y * 0.3) % 40)
            if value < 20:
                r, g, b = 139 + value, 90 + value//2, 43
            else:
                r, g, b = 139 - (value-20), 90 - (value-20)//2, 43
            pixels[x, y] = (r, g, b)
    
    return img

def create_metal_texture(size=512):
    """Vytvoří kovovou texturu se šedými tóny"""
    img = Image.new('RGB', (size, size))
    pixels = img.load()
    
    for y in range(size):
        for x in range(size):
            # Kovový efekt s šumem
            base = 140
            noise = random.randint(-30, 30)
            value = min(255, max(0, base + noise + (x + y) % 10))
            pixels[x, y] = (value, value, value + 10)
    
    return img

def create_asphalt_texture(size=512):
    """Vytvoří asfaltovou texturu"""
    img = Image.new('RGB', (size, size))
    pixels = img.load()
    
    for y in range(size):
        for x in range(size):
            # Tmavě šedá s náhodným šumem
            noise = random.randint(-20, 20)
            value = min(255, max(0, 50 + noise))
            pixels[x, y] = (value, value, value)
    
    return img

if __name__ == "__main__":
    print("Vytváření testovacích textur...")
    
    # Vytvoření textur
    formula_texture = create_formula_red_texture()
    wood_texture = create_wood_texture()
    metal_texture = create_metal_texture()
    asphalt_texture = create_asphalt_texture()
    
    # Uložení textur
    formula_texture.save("formula_red.jpg", quality=90)
    print("✓ Vytvořena formula_red.jpg")
    
    wood_texture.save("wood.jpg", quality=90)
    print("✓ Vytvořena wood.jpg")
    
    metal_texture.save("metal.jpg", quality=90)
    print("✓ Vytvořena metal.jpg")
    
    asphalt_texture.save("asphalt.jpg", quality=90)
    print("✓ Vytvořena asphalt.jpg")
    
    print("\nVšechny textury byly úspěšně vytvořeny!")
    print("Umístěte je do složky: assets/textures/")
