# Implementace podpory textur v ZPG projektu

## Přehled změn

Byla přidána kompletní podpora pro načítání a použití textur v OpenGL projektu.

## Nové soubory

### 1. Texture.h / Texture.cpp
- Třída pro správu OpenGL textur
- Automatické načítání z obrazových souborů (JPG, PNG)
- Podpora mipmaps pro lepší kvalitu při různých vzdálenostech
- Správné uvolňování GPU paměti v destruktoru

### 2. stb_image.h
- Knihovna třetí strany pro načítání obrazových souborů
- Podporuje JPG, PNG, BMP, TGA a další formáty
- Single-header library (jednoduchá integrace)

## Upravené soubory

### 1. DrawableObject.h / DrawableObject.cpp
**Přidáno:**
- `std::shared_ptr<Texture> texture` - ukazatel na texturu
- `bool useTexture` - příznak, zda objekt používá texturu
- `void setTexture(std::shared_ptr<Texture>)` - metoda pro nastavení textury
- `bool hasTexture()` - kontrola, zda má objekt texturu
- `std::shared_ptr<Texture> getTexture()` - getter pro texturu

**Upraveno v draw():**
- Před vykreslením se textura navázá na texture unit 0
- Do shaderu se pošlou uniformy `useTexture` a `textureSampler`

### 2. advanced_lighting.frag
**Přidáno:**
- `uniform int useTexture` - příznak použití textury
- `uniform sampler2D textureSampler` - sampler pro texturu

**Upraveno:**
- V `main()` se nyní rozhoduje mezi texturou a barvou objektu
- Pro modely bez UV souřadnic se generují procedurální UV z world pozice
- Výsledná barva se vynásobí základní barvou (textura nebo objectColor)

### 3. ground.frag
**Přidáno:**
- Stejné uniformy jako v advanced_lighting.frag

**Upraveno:**
- Procedurální checker pattern lze nyní nahradit texturou
- Textura se opakuje (tiling) pomocou násobení UV souřadnic

### 4. Application.cpp
**Přidáno:**
- `#include "Texture.h"`

**Upraveno v createFormulaScene():**
- Vytvoření 4 textur (formula_red, wood, metal, asphalt)
- Přiřazení textur k různým objektům:
  - Formule 1 → červená textura
  - Dům → dřevěná textura
  - Kvádr → kovová textura
  - Podlaha → asfaltová textura

### 5. Project1.vcxproj
**Přidáno:**
- Texture.cpp do `<ClCompile>`
- Texture.h a stb_image.h do `<ClInclude>`

## Testovací textury

V složce `assets/textures/` byly vytvořeny 4 testovací textury:
1. **formula_red.jpg** - červená s šumem (pro formuli)
2. **wood.jpg** - dřevěný vzor s pruhy
3. **metal.jpg** - kovový povrch se šedými tóny
4. **asphalt.jpg** - tmavá asfaltová textura

Textury byly vygenerovány pomocí Python skriptu `generate_textures.py`.

## Použití v kódu

### Vytvoření textury:
```cpp
auto texture = std::make_shared<Texture>("cesta/k/texture.jpg");
```

### Přiřazení textury k objektu:
```cpp
drawableObject->setTexture(texture);
```

### Vykreslení se automaticky stará o:
- Navázání textury na správný texture unit
- Nastavení uniform proměnných v shaderu
- Použití správné barvy (textura vs. objectColor)

## Demonstrace

Ve scéně 7 (Formula scene - klávesa 7) lze vidět:
- **4 různé modely** s **4 různými texturami**
- Formule 1 s červenou texturou
- Dům s dřevěnou texturou
- Kvádr s kovovou texturou
- Podlaha s asfaltovou texturou

Každý objekt má správné osvětlení (Phong shading) aplikované na texturu.

## Technické detaily

### UV souřadnice
Protože některé modely nemají UV souřadnice, shader generuje jednoduché UV z world pozice:
```glsl
vec2 uv = vec2(fragWorldPos.x * 0.5 + 0.5, fragWorldPos.z * 0.5 + 0.5);
```

### Texture filtering
- MIN filter: `GL_LINEAR_MIPMAP_LINEAR` (trilineární filtrování)
- MAG filter: `GL_LINEAR` (lineární filtrování)
- Wrap mode: `GL_REPEAT` (opakování textury)

### Formát textur
- RGB nebo RGBA (detekce podle počtu kanálů)
- Automatické generování mipmaps
- Flip vertically při načítání (OpenGL konvence)

## Možná rozšíření

1. **UV souřadnice v modelech** - přidat skutečné UV do OBJ souborů
2. **Normal mapping** - přidat bump/normal mapy pro detailnější povrch
3. **Specular mapping** - různá lesklost na různých částech objektu
4. **Texture atlasy** - více textur v jednom souboru
5. **PBR textury** - physically based rendering (albedo, metallic, roughness)

## Závěr

Projekt nyní plně podporuje textury s jednoduchým API pro jejich použití. Textury fungují korektně se stávajícím osvětlovacím systémem a lze je snadno přidat k libovolnému DrawableObject.
