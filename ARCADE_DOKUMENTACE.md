# Jednoduchá 3D Arkáda - Dokumentace

## Popis aplikace

Jedná se o jednoduchou 3D arkádovou hru ve stylu "střílečky na cíle", kde hráč klikáním na pohybující se objekty získává body. Hra využívá pokročilé techniky 3D grafiky včetně transformací, osvětlení a identifikace objektů.

## Spuštění hry

1. Spusťte aplikaci
2. Stiskněte klávesu **8** pro přepnutí do arkádové scény
3. Klikejte levým tlačítkem myši na pohybující se cíle
4. Sledujte své skóre v konzoli

## Ovládání

- **Klávesa 8**: Přepnutí do arkádové scény
- **WASD**: Pohyb kamery
- **Pravé tlačítko myši + pohyb**: Rozhlížení
- **Levé tlačítko myši**: Střelba na cíl (v arkádové scéně)

## Herní mechanika

### Typy cílů

Hra obsahuje tři typy cílů s různým bodováním:

1. **Modrá koule (Sphere)**: 10 bodů
   - Nejčastější cíl, základní obtížnost
   
2. **Žlutá kostka (Cube)**: 20 bodů
   - Střední hodnota, střední obtížnost
   
3. **Červený dárek (Gift)**: 50 bodů
   - Vzácný cíl, nejvyšší hodnota

### Pohyb cílů

- Každý cíl se pohybuje po **lomené čáře** (parametricky zadané úsečce)
- Cesty jsou generovány **náhodně** při každém spuštění
- Každý cíl má svou vlastní rychlost pohybu
- Pohyb je cyklický (po dosažení konce cesty se cíl vrací na začátek)

## Důležité části z pohledu programování

### 1. Transformace pro pohyb po lomené čáře (`LinePathTransform.h`)

**Účel**: Umožňuje objektům pohybovat se po lomené čáře zadané seznamem bodů.

**Klíčové vlastnosti**:
- Dědí z `TransformComponent`
- Přijímá seznam bodů (`std::vector<glm::vec3>`)
- Používá lineární interpolaci mezi body
- Podporuje cyklický pohyb (loop) nebo jednorázový průchod

**Implementační detaily**:
```cpp
// Výpočet aktuální pozice na lomené čáře
glm::vec3 interpolate(float t) const {
    // t = normalizovaný čas (0.0 - 1.0)
    // Určíme segment a lokální pozici v něm
    // Lineární interpolace mezi dvěma body: p0 + t*(p1 - p0)
}
```

**Využití**:
- Transformace je dynamická - používá `glfwGetTime()` pro aktuální čas
- Každý frame se přepočítává pozice objektu
- Umožňuje plynulý pohyb bez skokového přepínání

### 2. Herní cíl (`Target.h`)

**Účel**: Rozšíření `DrawableObject` o herní logiku - typ cíle, bodovou hodnotu, aktivní stav.

**Klíčové vlastnosti**:
- Enum `TargetType` pro kategorizaci cílů
- Automatické nastavení bodové hodnoty podle typu
- Stav `active` pro sledování, zda byl cíl již zasažen

**Design pattern**: Využívá dědičnosti z `DrawableObject`, rozšiřuje funkcionalitu o herní data.

### 3. Správce hry (`ArcadeGame.h`)

**Účel**: Centrální správa herní logiky - skóre, registrace cílů, zpracování zásahů.

**Klíčové funkce**:

```cpp
int hitTarget(unsigned int targetID) {
    // 1. Najde cíl podle ID (stencil buffer identifikace)
    // 2. Zkontroluje, zda je aktivní
    // 3. Přičte body ke skóre
    // 4. Deaktivuje cíl
    // 5. Vrátí počet bodů
}
```

**Generování náhody**:
- Používá `std::mt19937` pro kvalitní náhodná čísla
- `generateRandomPath()` - vytváří náhodné cesty pro cíle
- `getRandomDuration()` - různé rychlosti pohybu
- Při každém spuštění aplikace se generují jiné cesty

### 4. Vytvoření scény (`Application.cpp::createArcadeScene()`)

**Postup**:

1. **Vytvoření pozadí/země**:
   ```cpp
   auto ground = std::make_unique<DrawableObject>(plain, programBg);
   ground->setTexture(grassTexture);
   ```

2. **Generování cílů**:
   ```cpp
   for (každý typ cíle) {
       // Vytvoř model podle typu
       auto target = std::make_unique<Target>(model, program, type);
       
       // Vygeneruj náhodnou cestu
       auto path = arcadeGame.generateRandomPath(...);
       
       // Přidej transformaci pohybu
       t->addTransformation(std::make_unique<LinePathTransform>(path, duration));
       
       // Registruj cíl ve hře
       arcadeGame.addTarget(target.get());
   }
   ```

3. **Nastavení osvětlení**:
   - Jasné ambientní světlo (0.7 intenzita)
   - Silné směrové světlo shora (simulace slunce)

### 5. Identifikace objektů (Stencil Buffer)

**Jak funguje klikání na cíle**:

1. **OpenGL Stencil Buffer**:
   - Každý objekt má unikátní ID (`setID()`)
   - Při vykreslování se ID zapisuje do stencil bufferu
   - `glReadPixels()` s `GL_STENCIL_INDEX` vrátí ID objektu pod kurzorem

2. **Zpracování kliknutí** (`handleMouseClick()`):
   ```cpp
   if (activeSceneIndex == 7) { // Arkádová scéna
       int points = arcadeGame.hitTarget(index);
       if (points > 0) {
           printf("*** ZASAH! +%d bodu! ***\n", points);
           // Vizuální feedback - změna barvy
       }
   }
   ```

### 6. Prvek náhody

**Implementace náhodnosti**:

```cpp
// V ArcadeGame konstruktoru:
std::mt19937 rng(std::random_device{}());

// Při generování cesty:
std::uniform_real_distribution<float> distX(minX, maxX);
float randomX = distX(rng);
```

**Co je náhodné**:
- Počáteční pozice cílů
- Trasy pohybu (lomené čáry s 3-5 body)
- Rychlost pohybu (3-8 sekund na cestu)
- Při každém spuštění aplikace budou cíle na jiných místech a s jinými cestami

## Technické detaily

### Transformační hierarchie

```
TransformComposite (kontejner transformací)
  └─ LinePathTransform (pohyb po cestě - dynamický, závislý na čase)
     └─ Scale (velikost objektu - statický)
```

### Rendering pipeline

1. **Update fáze**:
   - `LinePathTransform::getMatrix()` vypočítá aktuální pozici
   - Transformační matice se zkombinují (path * scale)

2. **Draw fáze**:
   - První průchod: vykreslení s zapisem ID do stencil bufferu
   - Objekty se vykreslí s Phong shadingem
   - Lighting: ambient + directional

3. **Input fáze**:
   - Detekce kliknutí myši
   - Čtení stencil bufferu → získání ID objektu
   - `ArcadeGame::hitTarget()` → zpracování zásahu

### Integrace do existujícího systému

Hra využívá již existující infrastrukturu:

- **Camera**: Pohyb a rozhlížení (WASD + myš)
- **ShaderProgram**: Osvětlovací model (Phong)
- **Scene**: Správa objektů
- **DrawableObject**: Vykreslování 3D modelů
- **Stencil buffer**: Identifikace objektů (již implementováno)

Přidáno pouze:
- `LinePathTransform` - nová transformace
- `Target` - wrapper pro herní cíle
- `ArcadeGame` - správce herní logiky
- `createArcadeScene()` - generování scény

## Možná rozšíření

1. **Časový limit**: Přidat odpočítávání času
2. **Životy**: Omezený počet pokusů
3. **Combo systém**: Bonusové body za rychlé po sobě jdoucí zásahy
4. **Obtížnost**: Postupně zrychlovat cíle
5. **Power-upy**: Speciální objekty s bonusy (zpomalení času, multi-hit)
6. **Bézierovy křivky**: Místo lomených čar použít hladší křivky (jak bylo zmíněno v zadání)
