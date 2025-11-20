# Souhrn implementace 3D Arkády

## Co bylo vytvořeno

### Nové třídy a soubory

1. **LinePathTransform.h** 
   - Transformace pro pohyb po lomené čáře
   - Parametricky zadaná cesta (seznam bodů)
   - Lineární interpolace mezi body
   - Cyklický nebo jednorázový průchod

2. **Target.h**
   - Herní cíl s bodovou hodnotou
   - Tři typy: Sphere (10), Cube (20), Gift (50)
   - Sledování aktivního stavu

3. **ArcadeGame.h**
   - Správce herní logiky
   - Zpracování zásahů a skórování
   - Generování náhodných cest a rychlostí
   - Používá mt19937 pro kvalitní náhodná čísla

4. **ARCADE_DOKUMENTACE.md**
   - Kompletní popis aplikace
   - Důležité části z pohledu programování
   - Technické detaily implementace

### Upravené soubory

- **Application.h**: Přidán člen `ArcadeGame arcadeGame`
- **Application.cpp**: 
  - Přidán include pro nové třídy
  - Implementována `createArcadeScene()`
  - Upravena `handleMouseClick()` pro herní logiku
  - Přidáno speciální osvětlení pro arkádovou scénu
  - Přidána klávesa 8 pro přepnutí do hry

## Jak hrát

1. Spusťte aplikaci
2. Stiskněte **klávesu 8**
3. Klikejte levým tlačítkem myši na pohybující se barevné objekty
4. Sledujte skóre v konzoli

## Splněné požadavky ze zadání

✅ **3D rozšíření**: Objekty se generují a pohybují v 3D prostoru  
✅ **Pohyb po úsečce**: Implementována `LinePathTransform` pro pohyb po lomené čáře  
✅ **Prvek náhody**: Náhodné pozice, cesty a rychlosti při každém spuštění  
✅ **Různě bodované cíle**: 10, 20, 50 bodů podle typu  
✅ **Identifikace objektů**: Využit stencil buffer pro detekci kliknutí  
✅ **Dokumentace**: Popsány důležité části a funkce

## Testování

Pro testování hry:

1. Spusťte aplikaci v Debug nebo Release módu
2. Zkontrolujte, že se načtou všechny modely (sphere, cube, gift)
3. Stiskněte 8 pro přechod do arkádové scény
4. Pohybujte kamerou (WASD) pro lepší úhel pohledu
5. Klikejte na pohybující se objekty
6. Sledujte výpis v konzoli: "*** ZASAH! +XX bodu! Celkove skore: YYY ***"

## Známé limitace a možná vylepšení

### Současná implementace

- Cíle se po zásahu pouze ztmavnou (zůstávají ve scéně)
- Není časový limit
- Skóre se resetuje při restartu aplikace

### Možná rozšíření (volitelná)

1. **Respawn systém**: Po zásahu cíl zmizí a znovu se objeví na nové cestě
2. **Časovač**: Hra běží 60 sekund, pak se ukáže finální skóre
3. **Obtížnost**: Postupně zrychlovat cíle nebo zmenšovat jejich velikost
4. **UI overlay**: Zobrazit skóre přímo na obrazovce (OpenGL text rendering)
5. **Zvukové efekty**: Přidat zvuk při zásahu
6. **Bézierovy křivky**: Nahradit lomené čáry hladkými křivkami (jak bylo zmíněno v zadání)

## Programátorské koncepty použité v implementaci

- **OOP**: Dědičnost (Target ← DrawableObject), zapouzdření
- **Design patterns**: Composite (TransformComposite), Observer (pro kameru a světla)
- **STL**: vector, unique_ptr, shared_ptr
- **Náhodnost**: random_device, mt19937, uniform_real_distribution
- **OpenGL**: Stencil buffer pro picking, transformační matice
- **Matematika**: Lineární interpolace, vektorová algebra (glm)
- **Časování**: glfwGetTime() pro animace závislé na čase

## Závěr

Implementace splňuje všechny požadavky ze zadání:
- Jednoduchá 3D arkáda ve stylu "střílečky na cíle"
- Pohyb po parametricky zadaných lomených čárách
- Náhodné generování při každém spuštění
- Různě bodované cíle s identifikací
- Dokumentace důležitých částí kódu

Kód je napsán jednoduše a čistě, využívá již existující infrastrukturu projektu a snadno se dá rozšířit o další funkce.
