# Rychlý návod - Použití textur

## Jak přidat texturu k objektu

### Krok 1: Vytvořit texturu
```cpp
auto myTexture = std::make_shared<Texture>("assets/textures/moje_textura.jpg");
```

### Krok 2: Přiřadit k objektu
```cpp
drawableObject->setTexture(myTexture);
```

### Hotovo!
Objekt se nyní vykreslí s texturou místo jednobarevné barvy.

## Spuštění programu

1. Zkompilujte projekt v Visual Studio (F5 nebo Ctrl+F5)
2. Stiskněte klávesu **7** pro přepnutí na scénu s formulí
3. Uvidíte 4 různé objekty se 4 různými texturami:
   - Formule 1 (červená)
   - Dům (dřevo)
   - Kvádr (kov)
   - Podlaha (asfalt)

## Ovládání

- **Klávesy 1-7**: Přepínání mezi scénami
- **WASD**: Pohyb kamery
- **Pravé tlačítko myši + pohyb**: Otáčení kamery
- **F**: Zapnutí/vypnutí baterky
- **Q/E/R**: Změna FOV (45°/90°/130°)

## Vlastní textury

Pokud chcete použít vlastní textury:

1. Umístěte JPG nebo PNG soubor do složky `assets/textures/`
2. V kódu vytvořte texturu s cestou k vašemu souboru
3. Přiřaďte ji k objektu pomocí `setTexture()`

## Podporované formáty

- JPG / JPEG
- PNG (včetně průhlednosti)
- BMP
- TGA
- A další (viz stb_image dokumentace)

## Poznámky

- Textury jsou automaticky zmenšeny na vhodnou velikost
- Používají se mipmaps pro lepší kvalitu
- Pokud textura neexistuje, program ji přeskočí (vypíše chybu) a použije barvu
