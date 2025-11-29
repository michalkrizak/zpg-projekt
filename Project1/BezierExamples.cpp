// Příklady použití Bézierovy křivky a spline v projektu
// Tyto příklady ukážou, jak vytvořit různé typy pohybu

// =============================================================================
// PŘÍKLAD 1: Jednoduchá O-křivka (základ)
// =============================================================================

void createSimpleOCurve() {
    // Vytvoření jednoduché O-křivky se 4 kontrolními body
    glm::vec3 p0(-3.0f, 0.0f, 0.0f);   // Levý bod
    glm::vec3 p1(-3.0f, 2.0f, 0.0f);   // Horní levý kontrolní bod
    glm::vec3 p2(3.0f, 2.0f, 0.0f);    // Horní pravý kontrolní bod
    glm::vec3 p3(3.0f, 0.0f, 0.0f);    // Pravý bod
    
    auto bezier = std::make_unique<BezierCurveTransform>(
        p0, p1, p2, p3,
        5.0f,   // 5 sekund na projití
        true,   // opakovat
        true    // rotace podle směru
    );
}

// =============================================================================
// PŘÍKLAD 2: S-křivka (serpentina)
// =============================================================================

void createSCurve() {
    // S-křivka pro simulaci serpentin na závodní dráze
    glm::vec3 p0(-5.0f, 0.0f, -5.0f);  // Start dole vlevo
    glm::vec3 p1(-2.0f, 0.0f, 0.0f);   // Zakřivení doprava
    glm::vec3 p2(2.0f, 0.0f, 0.0f);    // Zakřivení doleva
    glm::vec3 p3(5.0f, 0.0f, 5.0f);    // Konec nahoře vpravo
    
    auto bezier = std::make_unique<BezierCurveTransform>(
        p0, p1, p2, p3,
        8.0f,
        false,  // nepřetáčet (jednosměrná cesta)
        true
    );
}

// =============================================================================
// PŘÍKLAD 3: Závodní okruh (spline s více segmenty)
// =============================================================================

void createRacingCircuit() {
    // Závodní okruh se 3 segmenty (10 kontrolních bodů)
    std::vector<glm::vec3> circuit = {
        // Segment 1: Rovinka
        glm::vec3(-10.0f, 0.0f, 0.0f),
        glm::vec3(-7.0f, 0.0f, 0.0f),
        glm::vec3(-4.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        
        // Segment 2: Zatáčka vpravo
        glm::vec3(2.0f, 0.0f, -3.0f),
        glm::vec3(5.0f, 0.0f, -6.0f),
        glm::vec3(8.0f, 0.0f, -8.0f),
        
        // Segment 3: Návrat
        glm::vec3(5.0f, 0.0f, -5.0f),
        glm::vec3(0.0f, 0.0f, -2.0f),
        glm::vec3(-5.0f, 0.0f, 0.0f)
    };
    
    auto spline = std::make_unique<BezierSplineTransform>(
        circuit,
        20.0f,  // 20 sekund na celý okruh
        true,   // opakovat dokola
        true
    );
}

// =============================================================================
// PŘÍKLAD 4: Výškový profil (stoupání a klesání)
// =============================================================================

void createHillyPath() {
    // Cesta s kopci - vertikální změny
    std::vector<glm::vec3> hills = {
        // Do kopce
        glm::vec3(-8.0f, -1.0f, 0.0f),
        glm::vec3(-6.0f, 0.0f, 0.0f),
        glm::vec3(-4.0f, 1.0f, 0.0f),
        glm::vec3(-2.0f, 1.5f, 0.0f),
        
        // Z kopce
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(2.0f, 0.0f, 0.0f),
        glm::vec3(4.0f, -0.5f, 0.0f),
        
        // Opět do kopce
        glm::vec3(6.0f, 0.5f, 0.0f),
        glm::vec3(8.0f, 1.5f, 0.0f),
        glm::vec3(10.0f, 2.0f, 0.0f)
    };
    
    auto spline = std::make_unique<BezierSplineTransform>(
        hills,
        15.0f,
        true,
        true
    );
}

// =============================================================================
// PŘÍKLAD 5: Spirála (3D spirálová cesta)
// =============================================================================

void createSpiralPath() {
    // Spirála stoupající vzhůru
    std::vector<glm::vec3> spiral;
    
    float radius = 5.0f;
    float height = 0.0f;
    int numPoints = 13; // Pro 3 segmenty
    
    for (int i = 0; i < numPoints; ++i) {
        float angle = (float)i * 3.14159f * 0.5f; // 90° mezi body
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        height += 0.3f; // Stoupání
        
        spiral.push_back(glm::vec3(x, height, z));
    }
    
    auto spline = std::make_unique<BezierSplineTransform>(
        spiral,
        25.0f,
        true,
        true
    );
}

// =============================================================================
// PŘÍKLAD 6: Osmička (křížení cest)
// =============================================================================

void createFigureEight() {
    // Cesta ve tvaru osmičky
    std::vector<glm::vec3> eight = {
        // Horní smyčka
        glm::vec3(0.0f, 2.0f, 0.0f),
        glm::vec3(-2.0f, 3.0f, 0.0f),
        glm::vec3(-2.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),  // Střed
        
        // Dolní smyčka
        glm::vec3(2.0f, -1.0f, 0.0f),
        glm::vec3(2.0f, -3.0f, 0.0f),
        glm::vec3(0.0f, -2.0f, 0.0f),
        
        // Návrat na začátek
        glm::vec3(-1.0f, -0.5f, 0.0f),
        glm::vec3(-1.0f, 1.5f, 0.0f),
        glm::vec3(0.0f, 2.0f, 0.0f)
    };
    
    auto spline = std::make_unique<BezierSplineTransform>(
        eight,
        18.0f,
        true,
        true
    );
}

// =============================================================================
// PŘÍKLAD 7: Komplexní scéna s více objekty na různých křivkách
// =============================================================================

void createMultiObjectScene() {
    auto scene = std::make_unique<Scene>();
    auto program = /* ... vytvoření shader programu ... */;
    
    // Objekt 1: Rychlá formule na vnější dráze
    if (auto model = Model::loadFromOBJ("assets/formula1.obj")) {
        auto obj = std::make_unique<DrawableObject>(std::move(model), program);
        
        std::vector<glm::vec3> outerTrack = {
            glm::vec3(-8.0f, 0.0f, -8.0f),
            glm::vec3(-8.0f, 0.0f, 8.0f),
            glm::vec3(0.0f, 0.0f, 10.0f),
            glm::vec3(8.0f, 0.0f, 8.0f),
            glm::vec3(8.0f, 0.0f, -8.0f),
            glm::vec3(0.0f, 0.0f, -10.0f),
            glm::vec3(-8.0f, 0.0f, -8.0f)
        };
        
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<BezierSplineTransform>(
            outerTrack, 12.0f, true, true
        ));
        t->addTransformation(std::make_unique<Scale>(0.02f, 0.02f, 0.02f));
        obj->getTransform().addTransformation(std::move(t));
        
        // Materiál: lesklý červený lak
        obj->setMaterial(0.2f, 0.8f, 0.9f, 128.0f);
        obj->setColor(glm::vec3(0.9f, 0.1f, 0.1f));
        
        scene->addObject(std::move(obj));
    }
    
    // Objekt 2: Pomalejší model na vnitřní dráze
    if (auto model = Model::loadFromOBJ("assets/cube.obj")) {
        auto obj = std::make_unique<DrawableObject>(std::move(model), program);
        
        std::vector<glm::vec3> innerTrack = {
            glm::vec3(-4.0f, 0.5f, -4.0f),
            glm::vec3(-4.0f, 0.5f, 4.0f),
            glm::vec3(0.0f, 0.5f, 5.0f),
            glm::vec3(4.0f, 0.5f, 4.0f),
            glm::vec3(4.0f, 0.5f, -4.0f),
            glm::vec3(0.0f, 0.5f, -5.0f),
            glm::vec3(-4.0f, 0.5f, -4.0f)
        };
        
        auto t = std::make_unique<TransformComposite>();
        t->addTransformation(std::make_unique<BezierSplineTransform>(
            innerTrack, 20.0f, true, true  // Pomalejší
        ));
        t->addTransformation(std::make_unique<Scale>(0.5f, 0.5f, 0.5f));
        obj->getTransform().addTransformation(std::move(t));
        
        // Materiál: matný modrý plast
        obj->setMaterial(0.3f, 0.9f, 0.3f, 32.0f);
        obj->setColor(glm::vec3(0.2f, 0.4f, 0.9f));
        
        scene->addObject(std::move(obj));
    }
}

// =============================================================================
// PŘÍKLAD 8: Různé materiály pro různé typy vozidel
// =============================================================================

void setupVehicleMaterials() {
    DrawableObject* formula;    // Ukazatel na objekt formule
    DrawableObject* truck;      // Ukazatel na objekt náklaďáku
    DrawableObject* bike;       // Ukazatel na objekt motorky
    
    // Formule: vysoce lesklý závodní lak
    // Ka (ambient), Kd (diffuse), Ks (specular), shininess
    formula->setMaterial(0.2f, 0.8f, 0.95f, 200.0f);
    
    // Náklaďák: matná barva s mírným leskem
    truck->setMaterial(0.3f, 0.9f, 0.2f, 16.0f);
    
    // Motorka: chromové části (vysoký specular)
    bike->setMaterial(0.1f, 0.5f, 1.0f, 256.0f);
}

// =============================================================================
// PŘÍKLAD 9: Dynamické přidávání bodů za běhu
// =============================================================================

class DynamicSplineBuilder {
    std::vector<glm::vec3> points;
    std::shared_ptr<BezierSplineTransform> spline;
    
public:
    void addPoint(const glm::vec3& point) {
        points.push_back(point);
        
        // Pokud máme alespoň 4 body, můžeme vytvořit křivku
        if (points.size() >= 4) {
            spline = std::make_shared<BezierSplineTransform>(
                points, 
                10.0f * (points.size() / 4),  // Delší čas pro více segmentů
                true, 
                true
            );
        }
    }
    
    bool canCreateSpline() const {
        return points.size() >= 4;
    }
    
    int getSegmentCount() const {
        return (points.size() - 1) / 3;
    }
};

// =============================================================================
// PŘÍKLAD 10: Optimalizace pro výkon
// =============================================================================

void performanceOptimization() {
    // Pro velké množství objektů na křivkách:
    
    // 1. Sdílení spline transformací
    auto sharedSpline = std::make_shared<BezierSplineTransform>(
        /* kontrolní body */, 15.0f, true, true
    );
    
    // Použití pro více objektů (všechny sledují stejnou cestu)
    // Poznámka: Každý objekt potřebuje vlastní kopii pro nezávislý pohyb
    
    // 2. Použití jednodušších křivek tam, kde stačí
    // BezierCurveTransform je rychlejší než BezierSplineTransform
    
    // 3. Vypnutí rotace, pokud není potřeba
    auto simpleSpline = std::make_unique<BezierSplineTransform>(
        /* body */, 10.0f, true, false  // orientToDirection = false
    );
}
