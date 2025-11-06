# Implementace Skybox (Cubemap)

## Přehled

Do scény s formulí (scéna 7) byl přidán skybox pomocí cubemap textury. Skybox vytváří iluzi nekonečného prostředí kolem scény.

## Nové soubory

### 1. Skybox.h / Skybox.cpp
- Třída pro správu skyboxu
- Načítá 6 textur (posx, negx, posy, negy, posz, negz)
- Vytváří kostku, která se renderuje kolem kamery
- Automaticky odstraňuje translaci z view matice (skybox je vždy ve středu)

### 2. skybox.vert / skybox.frag
- Vertex shader pro skybox
- Fragment shader pro vzorkování cubemap textury
- Používá speciální trick `gl_Position = pos.xyww` pro umístění na far plane

## Upravené soubory

### Scene.h / Scene.cpp
**Přidáno:**
- `std::unique_ptr<Skybox> skybox` - ukazatel na skybox
- `void setSkybox(std::unique_ptr<Skybox>)` - metoda pro nastavení skyboxu
- `void drawAll(viewPos, view, projection)` - nová metoda pro vykreslení se skyboxem

**Změny:**
- Skybox se vykresluje PRVNÍ (před všemi objekty)
- Depth test je nastaven na `GL_LEQUAL` během vykreslování skyboxu

### Application.cpp
**Změny v createFormulaScene():**
- Vytvoření skybox shader programu
- Načtení 6 cubemap textur z `assets/textures/cubemap/`
- Vytvoření instance Skybox
- Přiřazení skyboxu ke scéně pomocí `scene->setSkybox()`

**Změny v run():**
- Pro scénu 7 (Formula scene) se volá nová metoda `drawAll()` s view a projection maticemi
- Umožňuje správné vykreslení skyboxu

### Project1.vcxproj
**Přidáno:**
- Skybox.cpp do `<ClCompile>`
- Skybox.h do `<ClInclude>`
- skybox.vert a skybox.frag do `<None>` (shadery)

## Cubemap textury

V složce `assets/textures/cubemap/` jsou 6 textur:
- **posx.jpg** - Pozitivní X strana (pravá)
- **negx.jpg** - Negativní X strana (levá)
- **posy.jpg** - Pozitivní Y strana (horní)
- **negy.jpg** - Negativní Y strana (spodní)
- **posz.jpg** - Pozitivní Z strana (přední)
- **negz.jpg** - Negativní Z strana (zadní)

## Technické detaily

### Načítání cubemap
```cpp
std::vector<std::string> cubemapFaces = {
    "assets/textures/cubemap/posx.jpg",  // +X
    "assets/textures/cubemap/negx.jpg",  // -X
    "assets/textures/cubemap/posy.jpg",  // +Y
    "assets/textures/cubemap/negy.jpg",  // -Y
    "assets/textures/cubemap/posz.jpg",  // +Z
    "assets/textures/cubemap/negz.jpg"   // -Z
};
```

### Vytvoření skyboxu
```cpp
auto skybox = std::make_unique<Skybox>(cubemapFaces, skyboxProgram);
scene->setSkybox(std::move(skybox));
```

### Vykreslování
1. Depth function se nastaví na `GL_LEQUAL`
2. Z view matice se odstraní translace (skybox je vždy ve středu)
3. Cubemap textura se navíže na texture unit 0
4. Vykreslí se kostka (36 vertexů)
5. Depth function se vrátí na `GL_LESS`

### Shader trik
Vertex shader používá `gl_Position = pos.xyww` místo standardního `pos`. Tím se zajistí, že skybox bude vždy na far plane (maximální hloubka), takže všechny ostatní objekty budou před ním.

## Použití

### Spuštění programu
1. Zkompilujte projekt (F5)
2. Stiskněte klávesu **7** pro scénu s formulí
3. Rozhlédněte se pomocí myši - uvidíte skybox kolem sebe

### Ovládání kamery
- **Pravé tlačítko myši + pohyb**: Otáčení pohledu
- **WASD**: Pohyb kamery
- Skybox se pohybuje s kamerou (je vždy ve středu)

## Výhody skybox

1. **Iluze nekonečného prostoru** - vypadá to, jako by byl svět mnohem větší
2. **Realistické pozadí** - místo jednobarevného pozadí máte oblohu/prostředí
3. **Nízké náklady** - pouze jedna kostka navíc
4. **Reflexe** - může být použit i pro reflexe na kovových površích (pokročilé)

## Možná rozšíření

1. **Reflexe na objektech** - použít cubemap pro environment mapping
2. **HDR skybox** - pro realistické osvětlení
3. **Dynamický skybox** - měnit podle denní doby
4. **Částečková obloha** - přidat mraky nebo hvězdy jako overlay
5. **Procedurální obloha** - generovat oblohu v shaderu místo textur

## Poznámky

- Skybox musí být vykreslován PRVNÍ nebo POSLEDNÍ
- V této implementaci je první (aby se využil depth buffer)
- Cubemap textury by měly být stejně velké (doporučeno 1024x1024)
- Textury by měly navazovat na okrajích pro bezešvý přechod
- `stbi_set_flip_vertically_on_load(false)` je důležité pro cubemapy!

## Závěr

Skybox byl úspěšně implementován do scény s formulí. Poskytuje realistické prostředí a iluzi nekonečného světa. Integrace byla provedena minimálně invazivně - ovlivňuje pouze scénu 7.
