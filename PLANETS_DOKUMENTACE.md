# Dokumentace planetární scény

## Popis implementace

Scéna zobrazuje jednoduchou sluneční soustavu s **5 nebeskými tělesy** s realistickými texturami od NASA a fyzikálně inspirovaným pohybem.

## Použité textury

Všechny textury jsou umístěny ve složce `assets/textures/planets/`:
- `sun.jpg` - Slunce
- `mercury.jpg` - Merkur
- `earth.jpg` - Země
- `moon.jpg` - Měsíc
- `mars.jpg` - Mars

## Implementované objekty

### 1. Slunce
- **Pozice**: Stacionární ve středu soustavy (0, 0, 0)
- **Velikost**: 1.5× větší než standardní koule (scale = 1.5)
- **Rotace kolem vlastní osy**: 
  - Rychlost: 0.1 rad/s (velmi pomalá)
  - Osa: Y (0, 1, 0)
- **Orbitální pohyb**: Žádný (středový bod soustavy)
- **Shader**: `constant.frag` (žádné stínování, vždy plně osvětlené)
- **Textura**: `sun.jpg`

### 2. Merkur
- **Velikost**: 0.25× standardní koule
- **Orbitální vzdálenost**: 2.5 jednotek od Slunce
- **Rychlost oběhu**: 1.2 rad/s (nejrychlejší oběh)
- **Rotace kolem vlastní osy**: 
  - Rychlost: 0.4 rad/s
  - Osa: Y (0, 1, 0)
- **Shader**: `color_phong.frag` (Phongovo osvětlení)
- **Textura**: `mercury.jpg`
- **Poznámka**: Merkur má nejkratší oběžnou dráhu a nejrychleji obíhá Slunce

### 3. Země
- **Velikost**: 0.5× standardní koule
- **Orbitální vzdálenost**: 5.0 jednotek od Slunce
- **Rychlost oběhu kolem Slunce**: 
  - 0.6 rad/s (střední rychlost)
  - Osa rotace: Y (0, 1, 0)
  - Směr: proti směru hodinových ručiček (pohled shora)
- **Rotace kolem vlastní osy**: 
  - Rychlost: 2.0 rad/s (rychlá rotace - simuluje 24hodinový den)
  - Osa: Y (0, 1, 0)
- **Shader**: `color_phong.frag` (Phongovo osvětlení)
- **Textura**: `earth.jpg`
- **Popis pohybu**: Země obíhá kolem Slunce po kruhové dráze a zároveň se otáčí kolem své osy, což simuluje střídání dne a noci

### 4. Měsíc
- **Velikost**: 0.15× standardní koule (nejmenší těleso)
- **Orbitální vzdálenost**: 1.2 jednotek od Země
- **Rychlost oběhu kolem Země**: 
  - 3.0 rad/s (rychlý oběh - jeden lunární měsíc)
  - Osa rotace: Y (0, 1, 0)
- **Rychlost oběhu kolem Slunce**: 
  - Stejná jako Země (0.6 rad/s), protože se pohybuje společně se Zemí
- **Rotace kolem vlastní osy**: 
  - Rychlost: 0.5 rad/s (pomalá)
  - Osa: Y (0, 1, 0)
- **Shader**: `color_phong.frag` (Phongovo osvětlení)
- **Textura**: `moon.jpg`
- **Popis pohybu**: Měsíc má složený pohyb:
  1. Nejprve se pohybuje společně se Zemí kolem Slunce
  2. Zároveň obíhá kolem Země po vlastní kruhové dráze
  3. A navíc se otáčí kolem své vlastní osy

### 5. Mars
- **Velikost**: 0.4× standardní koule
- **Orbitální vzdálenost**: 7.5 jednotek od Slunce
- **Rychlost oběhu**: 0.4 rad/s (pomalejší než Země)
- **Rotace kolem vlastní osy**: 
  - Rychlost: 1.9 rad/s (podobná Zemi)
  - Osa: Y (0, 1, 0)
- **Shader**: `color_phong.frag` (Phongovo osvětlení)
- **Textura**: `mars.jpg`
- **Poznámka**: Mars má delší oběžnou dráhu, proto se pohybuje pomaleji

## Detailní popis transformací (na příkladu Slunce, Země a Měsíce)

### Slunce
```cpp
TransformComposite:
1. DynamicRotate(0.1f, 0, 1, 0)  // Rotace kolem osy Y
2. Scale(1.5, 1.5, 1.5)           // Zvětšení
```
**Výsledek**: Slunce zůstává ve středu a jen se pomalu otáčí kolem své osy.

### Země
```cpp
TransformComposite:
1. DynamicRotate(0.6f, 0, 1, 0)   // Orbitální rotace kolem Slunce
2. Translate(5.0, 0, 0)           // Posun na oběžnou dráhu
3. DynamicRotate(2.0f, 0, 1, 0)   // Rotace kolem vlastní osy
4. Scale(0.5, 0.5, 0.5)           // Zmenšení
```
**Výsledek**: 
- Transformace se aplikují v pořadí 1→2→3→4
- Nejprve se aplikuje rotace celého systému kolem Slunce (orbit)
- Pak se posune na vzdálenost 5 jednotek
- Následně se aplikuje vlastní rotace (den/noc)
- Nakonec se upraví velikost

### Měsíc
```cpp
TransformComposite:
1. DynamicRotate(0.6f, 0, 1, 0)   // Pohyb se Zemí kolem Slunce
2. Translate(5.0, 0, 0)           // Posun na pozici Země
3. DynamicRotate(3.0f, 0, 1, 0)   // Orbitální rotace kolem Země
4. Translate(1.2, 0, 0)           // Posun od Země na vlastní dráhu
5. DynamicRotate(0.5f, 0, 1, 0)   // Rotace kolem vlastní osy
6. Scale(0.15, 0.15, 0.15)        // Zmenšení
```
**Výsledek**: 
- Nejprve se Měsíc pohybuje spolu se Zemí kolem Slunce (kroky 1-2)
- Pak obíhá kolem Země (kroky 3-4)
- A nakonec se otáčí kolem své osy (krok 5)
- Výsledkem je složitá dráha epicykloidy

## Ovládání

- **Klávesa 4**: Přepnutí na planetární scénu
- **W/A/S/D**: Pohyb kamery
- **Pravé tlačítko myši + pohyb**: Otáčení kamery

## Technické poznámky

### Systém transformací
Použit hierarchický systém transformací `TransformComposite` s těmito typy transformací:
- `DynamicRotate`: Rotace závislá na čase (pro animaci)
- `Translate`: Statický posun v prostoru
- `Scale`: Změna velikosti

### Osvětlení
- Všechny planety (kromě Slunce) používají Phongův osvětlovací model
- Hlavní světlo (`mainLight`) je umístěno vysoko nad scénou
- Slunce používá `constant.frag` shader, takže svítí samo o sobě bez ohledu na osvětlení

### Fyzikální aproximace
Rychlosti a vzdálenosti jsou upraveny pro vizuální atraktivitu a nejsou ve skutečném měřítku:
- **Rychlost rotací**: Výrazně zvýšena pro viditelný efekt
- **Velikosti**: Upraveny pro lepší viditelnost (Slunce by bylo ve skutečnosti mnohem větší)
- **Vzdálenosti**: Zmenšeny, aby byly planety viditelné v jednom záběru
- **Směr rotace**: Všechny rotace jsou proti směru hodinových ručiček (pohled shora)

## Implementace

Planetární scéna je implementována v metodě `Application::createSolarScene()` v souboru `Application.cpp`.
