//----------------------------------------------------------------------|
// FACULTAD DE INGENIERÍA UNAM
// COMPUTACIÓN GRÁFICA E INTERACCIÓN HUMANO-COMPUTADORA
// 2026-2 | PROYECTO FINAL
// 13-mayo-2026
// PROFESOR: Ing. José Roque Román Guadarrama
// INTEGRANTES:
//   Pérez León Jesús Alexis
//   Rosas Cañada Abraham
//   Sánchez Meza Ariadna Osiris
//----------------------------------------------------------------------|

#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Skybox.h"
#include "Model.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "AudioManager.h"

const float toRadians = 3.14159265f / 180.0f;

// ======================================================
// VARIABLES GLOBALES — HUMO Y PISTA
// ======================================================

// Humo de las máscaras Aku/Uka
float rotHumoAku = 0.0f;
float creceHumoAltoAku = 0.0f;
float creceHumoAnchoAku = 0.0f;

// Humo escape carro 1 (Crash & Grunt)
float creceHumoCar1 = 0.0f;
float rotHumoCar1 = 0.0f;
float rotLlantaCar1 = 0.0f;
float offsetCentroPistaCar1 = -2.3f;
float offsetAlturaPistaCar1 = 0.0f;

// Humo escape carro 2 (Master Chief & Coco)
float creceHumoCar2 = 0.0f;
float rotHumoCar2 = 0.0f;
float rotLlantaCar2 = 0.0f;
float offsetCentroPistaCar2 = 1.9f;
float offsetAlturaPistaCar2 = -0.3f;

// Humo escape carro 3 (Harry Potter en moto)
float creceHumoCar3 = 0.0f;
float rotHumoCar3 = 0.0f;
float rotLlantaCar3 = 0.0f;
float offsetCentroPistaCar3 = 0.1f;
float offsetAlturaPistaCar3 = 2.0f;

// Datos de interpolación sobre la pista (compartidos entre los tres carros)
int   pistaIdx = 0;
float pistaT = 0.0f;
float velocidad = 0.5f;
float distanciaSegmento;
float velGral = 0.0f;

glm::vec3 posCars = glm::vec3(-41.6996f, 0.0f, 0.0f);
float angleY = 0.0f;   // Yaw  (giro horizontal)
float angleX = 0.0f;   // Pitch (inclinación en rampas)

bool animPista = false;

// ======================================================
// VARIABLES GLOBALES — ANIMACIÓN CRASH (APLASTA CAJAS)
// ======================================================

float rotacionTornado = 0.0f;
float velocidadGiro = 1000.0f;

enum EstadoAnimacion { REPOSO, TORNADO_CRASH, ROCA_RODANDO };
EstadoAnimacion estadoActual = REPOSO;

// Posiciones base de las cajas sobre el eje X
float inicioX = -40.0f;
float pasoCaja = 10.0f;

glm::vec3 posCajaAku = glm::vec3(inicioX, -2.0f, 80.0f);
glm::vec3 posCajaCaraCrash = glm::vec3(inicioX + pasoCaja, -2.0f, 80.0f);
glm::vec3 posCajaCheckPoint = glm::vec3(inicioX + pasoCaja * 2, -2.0f, 80.0f);
glm::vec3 posCajaFlechaArriba = glm::vec3(inicioX + pasoCaja * 3, -2.0f, 80.0f);
glm::vec3 posCajaMetalExcl = glm::vec3(inicioX + pasoCaja * 4, -2.0f, 80.0f);
glm::vec3 posCajaNitro = glm::vec3(inicioX + pasoCaja * 5, -2.0f, 80.0f);
glm::vec3 posCajaNitroExcl = glm::vec3(inicioX + pasoCaja * 6, -2.0f, 80.0f);
glm::vec3 posCajaNormal = glm::vec3(inicioX + pasoCaja * 7, -2.0f, 80.0f);
glm::vec3 posCajaQuestion = glm::vec3(inicioX + pasoCaja * 8, -2.0f, 80.0f);
glm::vec3 posCajaTNT = glm::vec3(inicioX + pasoCaja * 9, -2.0f, 80.0f);
glm::vec3 posTotem1Crash = glm::vec3(inicioX + pasoCaja * 11, -2.0f, 80.0f);

// Escalas dinámicas en Y para aplastamiento
float escAku = 5.f, escCara = 5.f, escCheck = 5.f, escFlecha = 5.f, escMetal = 5.f;
float escNitro = 5.f, escNitroExcl = 5.f, escNormal = 5.f, escQuest = 5.f, escTNT = 5.f;

// Bola de roca
glm::vec3 posBolaRocaInicial = glm::vec3(-55.0f, 4.0f, 80.0f);
glm::vec3 posBolaRoca = posBolaRocaInicial;
float rotacionRocaZ = 0.0f;
float velocidadRoca = 8.0f;

// Parámetros de cámara aérea
float aeroCamX = 45.0f;
float aeroCamZ = -45.0f;
float aeroCamAltura = 65.0f;
float aeroCamVel = 25.0f;

// ======================================================
// SISTEMA PRINCIPAL
// ======================================================

Window   mainWindow;
std::vector<Mesh*>   meshList;
std::vector<Shader>  shaderList;
Camera   camera;

Texture pisoTexture;
Texture humoTexture;
Skybox  skyboxDia;
Skybox  skyboxNoche;

AudioManager audioMgr;

// ======================================================
// MODELOS — ESCENARIO
// ======================================================
Model Pared_M;
Model PaloFarola_M;
Model FocoDer_M;
Model FocoIzq_M;
Model Banca_M;
Model BloquePasto_M;
Model BasePlanetas_M;
Model RelojBase_M;
Model RelojEngrane1_M;
Model RelojEngrane2_M;
Model RelojEngrane3_M;
Model RelojEngrane4_M;
Model RelojGalRedE_M;
Model RelojPlanetas_M;

// ======================================================
// MODELOS — AVATARES
// ======================================================
// Big Daddy (BioShock)
Model BigDaddyCuerpo_M;
Model BigDaddyTaladro_M;
Model BigDaddyBrazoIzq_M;
Model BigDaddyBrazoDer_M;
Model BigDaddyPiernaIzq_M;
Model BigDaddyPiernaDer_M;

// Hermione (Harry Potter)
Model Hermione_HP_M;
Model BrazoDer_HP_M;
Model BrazoIzq_HP_M;
Model PiernaDer_HP_M;
Model PiernaIzq_HP_M;

// Crash Bandicoot
Model Crash_Cuerpo_M;
Model Crash_BrazoDer_M;
Model Crash_BrazoIzq_M;
Model Crash_PiernaDer_M;
Model Crash_PiernaIzq_M;

// ======================================================
// MODELOS — OBJETOS BIOSHOCK
// ======================================================
Model BolsaDer_M;
Model BolsaIzq_M;
Model BolsaMid_M;
Model Plasmido_M;
Model Radio_M;

// ======================================================
// MODELOS — OBJETOS HARRY POTTER
// ======================================================
Model Carro_HP_M;
Model LlantaFD_HP_M;
Model LlantaFI_HP_M;
Model LlantaAD_HP_M;
Model LlantaAI_HP_M;
Model Bolsa_HP_M;
Model Giratiempo_HP_M;
Model LibroHechizos_HP_M;
Model Snitch_HP_M;
Model Copa_HP_M;
// Expreso de Hogwarts
Model ExpresoHowarts_M;
Model LlantaSteamCar3_M;
Model SteamCar3_M;

// ======================================================
// MODELOS — OBJETOS CRASH BANDICOOT
// ======================================================
Model Crash_GoKart_M;
Model LlantaFD_Crash_M;
Model LlantaFI_Crash_M;
Model LlantaAD_Crash_M;
Model LlantaAI_Crash_M;
Model SteamCar_Crash_M;
Model ArbolWumpa_M;
Model BaseCilindro_M;
Model BolaRoca_M;
Model CajaAku_M;
Model CajaCaraCrash_M;
Model CajaCheckPoint_M;
Model CajaFlechaArriba_M;
Model CajaMetalExclamation_M;
Model CajaNitro_M;
Model CajaNitroExclamation_M;
Model CajaNormalCrash_M;
Model CajaQuestion_M;
Model CajaTNT_M;
Model CocoOnGoKart_M;
Model CortexOnGoKart_M;
Model CrystalCrash_M;
Model CrystalHielo_M;
Model FrutaWumpa_M;
Model JoyasCrash_M;
Model MascaraAku_M;
Model MascaraAkuNoche_M;
Model MascaraUka_M;
Model Pista_M;
Model SteamCar1_M;
Model SteamCar2_M;
Model Totem1Crash_M;
Model Totem2Crash_M;
Model RuedaSteamCar2_M;

// ======================================================
// MATERIALES
// ======================================================
Material Material_brillante;   // Metales, vidrio, plasmido
Material Material_opaco;       // Madera, tela, pasto, pared

// ======================================================
// TIEMPO
// ======================================================
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// ======================================================
// LUCES
// ======================================================
DirectionalLight mainLight;
PointLight  pointLights[MAX_POINT_LIGHTS];
SpotLight   spotLights[MAX_SPOT_LIGHTS];

// ======================================================
// SHADERS
// ======================================================
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

// ======================================================
// SISTEMA DE CAPTURA DE WAYPOINTS
// C = activar/desactivar  |  P = guardar punto  |  L = limpiar lista
// ======================================================
bool modoCaptura = false;
int  capturaContador = 0;
std::vector<glm::vec3> waypointsCapturados;
bool teclaCAnterior = false;
bool teclaPAnterior = false;
bool teclaLAnterior = false;

// ======================================================
// HELPERS
// ======================================================

// Aplica Translate * RotX * RotY * RotZ * Scale sobre una matriz base
glm::mat4 ApplyTRS(glm::mat4 base, glm::vec3 t, glm::vec3 r, glm::vec3 s)
{
    base = glm::translate(base, t);
    base = glm::rotate(base, r.x * toRadians, glm::vec3(1, 0, 0));
    base = glm::rotate(base, r.y * toRadians, glm::vec3(0, 1, 0));
    base = glm::rotate(base, r.z * toRadians, glm::vec3(0, 0, 1));
    base = glm::scale(base, s);
    return base;
}

// Normalización segura (evita NaN con vectores nulos)
glm::vec3 SafeNormalize(const glm::vec3& v)
{
    float len = glm::length(v);
    return (len <= 0.0001f) ? glm::vec3(0.0f) : v / len;
}

// Mueve un objeto hacia el origen en XZ por un paso dado
glm::vec3 MoveTowardsOriginXZ(glm::vec3 p, float step)
{
    if (p.x > 0.0f) p.x -= step;
    else if (p.x < 0.0f) p.x += step;
    if (p.z > 0.0f) p.z -= step;
    else if (p.z < 0.0f) p.z += step;
    return p;
}

// Avanza un follower genérico por una lista de waypoints
void UpdatePathFollower(
    glm::vec3& pos, float& rotY, int& currentIndex,
    const std::vector<glm::vec3>& path,
    float speed, float dt, float threshold = 0.60f)
{
    if (path.empty()) return;

    glm::vec3 target = path[currentIndex];
    glm::vec3 delta = target - pos;
    float dist = glm::length(delta);

    if (dist < threshold) {
        currentIndex = (currentIndex + 1) % (int)path.size();
        target = path[currentIndex];
        delta = target - pos;
        dist = glm::length(delta);
    }
    if (dist > 0.0001f) {
        glm::vec3 dir = delta / dist;
        float step = speed * dt;
        if (step > dist) step = dist;
        pos += dir * step;
        rotY = atan2(dir.x, dir.z) / toRadians;
    }
}

// ======================================================
// GEOMETRÍA BASE (piso + planos de humo)
// ======================================================
void CreateObjects()
{
    // Piso plano con UV para tiling de textura
    unsigned int floorIndices[] = { 0,2,1, 1,2,3 };
    GLfloat floorVertices[] = {
        -10.f,0.f,-10.f,  0.f, 0.f,  0.f,1.f,0.f,
         10.f,0.f,-10.f, 10.f, 0.f,  0.f,1.f,0.f,
        -10.f,0.f, 10.f,  0.f,10.f,  0.f,1.f,0.f,
         10.f,0.f, 10.f, 10.f,10.f,  0.f,1.f,0.f
    };
    Mesh* floor = new Mesh();
    floor->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(floor);

    // Dos planos cruzados para simular humo en perspectiva
    unsigned int humoIndices[] = { 0,1,2, 0,2,3, 4,5,6, 4,6,7 };
    GLfloat humoVertices[] = {
        -0.5f,0.f,0.f,  0.f,0.f,  0.f,0.f,1.f,
         0.5f,0.f,0.f,  1.f,0.f,  0.f,0.f,1.f,
         0.5f,1.f,0.f,  1.f,1.f,  0.f,0.f,1.f,
        -0.5f,1.f,0.f,  0.f,1.f,  0.f,0.f,1.f,
         0.f,0.f,-0.5f, 0.f,0.f,  1.f,0.f,0.f,
         0.f,0.f, 0.5f, 1.f,0.f,  1.f,0.f,0.f,
         0.f,1.f, 0.5f, 1.f,1.f,  1.f,0.f,0.f,
         0.f,1.f,-0.5f, 0.f,1.f,  1.f,0.f,0.f,
    };
    Mesh* humo = new Mesh();
    humo->CreateMesh(humoVertices, humoIndices, 64, 12);
    meshList.push_back(humo);
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

// ======================================================
// PUNTOS DE LA PISTA (coordenadas Blender x, z=y)
// ======================================================
std::vector<glm::vec3> puntosPista = {
    {-41.6996f, 0.6028f,0.f}, {-41.7042f, 1.6028f,0.f},
    {-41.6725f, 3.2178f,0.f}, {-41.6772f, 5.3804f,0.f},
    {-41.6772f,25.8454f,0.f}, {-41.5318f,31.5867f,0.f},
    {-40.3448f,35.1235f,0.f}, {-38.2130f,38.0547f,0.f},
    {-35.1850f,40.2349f,0.f}, {-31.7208f,41.3250f,0.f},
    {-25.9796f,41.4461f,0.f}, {-25.9796f,41.4461f,0.f},
    { 28.3157f,41.4742f,0.f}, { 32.0447f,41.2919f,0.f},
    { 35.5354f,40.1284f,0.f}, { 38.5214f,37.9694f,0.f},
    { 40.6411f,34.9616f,0.f}, { 41.7918f,31.4288f,0.f},
    { 41.9129f,27.7749f,0.f}, { 41.9371f,-16.314f,0.f},
    { 41.7918f,-20.1415f,0.f},{ 40.5805f,-23.6541f,0.f},
    { 38.3599f,-26.5981f,0.f},{ 35.3655f,-28.7682f,0.f},
    { 31.8664f,-29.8953f,0.f},{ 28.0981f,-30.0131f,0.f},
    { 28.0981f,-30.0131f,0.f},{-30.0008f,-30.0131f,0.f},
    {-31.8176f,-29.8516f,0.f},{-35.3302f,-28.6807f,0.f},
    {-38.2775f,-26.5005f,0.f},{-38.2775f,-26.5005f,0.f},
    {-40.4376f,-23.5128f,0.f},{-41.1441f,-21.7363f,0.f},
    {-41.5479f,-20.0002f,0.f},{-41.5479f,-20.0002f,0.f},
    {-41.6690f,-16.2050f,0.f},{-41.6690f, -0.0889f,0.f},
};

// Interpolación lineal de los carros sobre la pista
void recorrerPista(float dt)
{
    if (puntosPista.empty()) return;

    glm::vec3 p0 = puntosPista[pistaIdx];
    int sig = (pistaIdx + 1) % (int)puntosPista.size();
    glm::vec3 p1 = puntosPista[sig];

    distanciaSegmento = glm::distance(p0, p1);
    if (distanciaSegmento > 0.0001f)
        pistaT += (velocidad * dt) / distanciaSegmento;
    else
        pistaT = 1.0f;

    if (pistaT >= 1.0f) {
        pistaT = 0.0f;
        pistaIdx = (pistaIdx + 1) % (int)puntosPista.size();
        return;
    }

    glm::vec3 posInterp = p0 + pistaT * (p1 - p0);
    posCars.x = posInterp.x;
    posCars.y = posInterp.z;  // eje Z Blender → Y OpenGL
    posCars.z = posInterp.y;  // eje Y Blender → Z OpenGL

    glm::vec3 dir = { p1.x - p0.x, p1.z - p0.z, p1.y - p0.y };
    if (glm::length(dir) > 0.0001f) {
        dir = glm::normalize(dir);
        angleY = glm::degrees(atan2(dir.x, dir.z));
        angleX = glm::degrees(-asinf(dir.y));
    }
}

// Secuencia completa: tornado de Crash → bola de roca aplasta cajas
void animacionAplastaCajas(float dt)
{
    switch (estadoActual) {
    case REPOSO:
        if (mainWindow.getsKeys()[GLFW_KEY_1]) {
            posBolaRoca = posBolaRocaInicial;
            rotacionRocaZ = 0.0f;
            escAku = escCara = escCheck = escFlecha = escMetal = 5.f;
            escNitro = escNitroExcl = escNormal = escQuest = escTNT = 5.f;
            estadoActual = TORNADO_CRASH;
            rotacionTornado = 0.0f;
            audioMgr.PlayAudio("roca", glm::vec3(0.f));
        }
        break;

    case TORNADO_CRASH:
        rotacionTornado += (dt * 10.f) * velocidadGiro;
        if (rotacionTornado >= 7200.f) {
            rotacionTornado = 0.f;
            estadoActual = ROCA_RODANDO;
        }
        break;

    case ROCA_RODANDO:
        posBolaRoca.x += dt * velocidadRoca;
        rotacionRocaZ -= dt * 100.0f;
        if (posBolaRoca.x > posCajaAku.x)          escAku = 0.5f;
        if (posBolaRoca.x > posCajaCaraCrash.x)    escCara = 0.5f;
        if (posBolaRoca.x > posCajaCheckPoint.x)   escCheck = 0.5f;
        if (posBolaRoca.x > posCajaFlechaArriba.x) escFlecha = 0.5f;
        if (posBolaRoca.x > posCajaMetalExcl.x)    escMetal = 0.5f;
        if (posBolaRoca.x > posCajaNitro.x)        escNitro = 0.5f;
        if (posBolaRoca.x > posCajaNitroExcl.x)    escNitroExcl = 0.5f;
        if (posBolaRoca.x > posCajaNormal.x)       escNormal = 0.5f;
        if (posBolaRoca.x > posCajaQuestion.x)     escQuest = 0.5f;
        if (posBolaRoca.x > posCajaTNT.x)          escTNT = 0.5f;
        if (posBolaRoca.x >= (posTotem1Crash.x - 9.0f)) {
            posBolaRoca.x = posTotem1Crash.x - 9.0f;
            ALuint src = audioMgr.sources["roca"];
            ALint state; alGetSourcei(src, AL_SOURCE_STATE, &state);
            if (state == AL_PLAYING) alSourceStop(src);
            estadoActual = REPOSO;
        }
        break;
    }
}

// ======================================================
// MAIN
// ======================================================
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    mainWindow = Window(1366, 768);
    mainWindow.Initialise();
    audioMgr.Init();
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    // ------------------------------------------------
    // CARGA DE AUDIO
    // ------------------------------------------------
    audioMgr.LoadWAV("ambiente", "Sounds/MenuSound.wav", 0.4f);
    alSourcei(audioMgr.sources["ambiente"], AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(audioMgr.sources["ambiente"], AL_POSITION, 0.f, 0.f, 0.f);

    audioMgr.LoadWAV("bigdaddy", "Sounds/SoundBigDad.wav", 0.6f);
    alSourcef(audioMgr.sources["bigdaddy"], AL_REFERENCE_DISTANCE, 8.0f);
    alSourcef(audioMgr.sources["bigdaddy"], AL_MAX_DISTANCE, 70.0f);
    alSourcef(audioMgr.sources["bigdaddy"], AL_ROLLOFF_FACTOR, 1.0f);

    audioMgr.LoadWAV("radio", "Sounds/RadioSound.wav", 0.8f);
    alSourcef(audioMgr.sources["radio"], AL_REFERENCE_DISTANCE, 5.0f);
    alSourcef(audioMgr.sources["radio"], AL_MAX_DISTANCE, 30.0f);
    alSourcef(audioMgr.sources["radio"], AL_ROLLOFF_FACTOR, 1.0f);
    alSourcei(audioMgr.sources["radio"], AL_LOOPING, AL_TRUE);

    audioMgr.LoadWAV("roca", "Sounds/Roca.wav", 1.0f);
    alSourcei(audioMgr.sources["roca"], AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3f(audioMgr.sources["roca"], AL_POSITION, 0.f, 0.f, 0.f);

    audioMgr.LoadWAV("trenInicio", "Sounds/TrenInicio.wav", 1.0f);
    audioMgr.LoadWAV("trenMov", "Sounds/TrenMov.wav", 1.0f);
    audioMgr.LoadWAV("trenFrenando", "Sounds/TrenFrenando.wav", 1.0f);
    for (const char* key : { "trenInicio","trenMov","trenFrenando" }) {
        alSourcei(audioMgr.sources[key], AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(audioMgr.sources[key], AL_POSITION, 0.f, 0.f, 0.f);
    }
    alSourcei(audioMgr.sources["trenMov"], AL_LOOPING, AL_TRUE);
    alSourcei(audioMgr.sources["trenInicio"], AL_LOOPING, AL_FALSE);
    alSourcei(audioMgr.sources["trenFrenando"], AL_LOOPING, AL_FALSE);

    // ------------------------------------------------
    // INICIALIZACIÓN
    // ------------------------------------------------
    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), -60.f, 0.f, 20.f, 0.3f);

    // ------------------------------------------------
    // TEXTURAS Y SKYBOX
    // ------------------------------------------------
    pisoTexture = Texture("Textures/piso2.jpg");
    pisoTexture.LoadTextureA();
    humoTexture = Texture("Textures/humo1.png");
    humoTexture.LoadTextureA();

    std::vector<std::string> facesDia = {
        "Textures/Skybox/_px.jpg","Textures/Skybox/_nx.jpg",
        "Textures/Skybox/_ny.jpg","Textures/Skybox/_py.jpg",
        "Textures/Skybox/_pz.jpg","Textures/Skybox/_nz.jpg"
    };
    std::vector<std::string> facesNoche = {
        "Textures/Skybox/_pxnight.jpg","Textures/Skybox/_nxnight.jpg",
        "Textures/Skybox/_nynight.jpg","Textures/Skybox/_pynight.jpg",
        "Textures/Skybox/_pznight.jpg","Textures/Skybox/_nznight.jpg"
    };
    skyboxDia = Skybox(facesDia);
    skyboxNoche = Skybox(facesNoche);

    // ------------------------------------------------
    // CARGA DE MODELOS
    // ------------------------------------------------
    printf("\n==== CARGANDO MODELOS ====\n");

    printf("\n================ ESCENARIO ================\n");

    printf("Cargando Pared_M -> Models/pared.obj ...\n");
    Pared_M = Model(); Pared_M.LoadModel("Models/pared.obj");
    printf("OK -> Pared_M\n");

    printf("Cargando PaloFarola_M -> Models/Palofarola.obj ...\n");
    PaloFarola_M = Model(); PaloFarola_M.LoadModel("Models/Palofarola.obj");
    printf("OK -> PaloFarola_M\n");

    printf("Cargando FocoDer_M -> Models/Focoder.obj ...\n");
    FocoDer_M = Model(); FocoDer_M.LoadModel("Models/Focoder.obj");
    printf("OK -> FocoDer_M\n");

    printf("Cargando FocoIzq_M -> Models/Focoizq.obj ...\n");
    FocoIzq_M = Model(); FocoIzq_M.LoadModel("Models/Focoizq.obj");
    printf("OK -> FocoIzq_M\n");

    printf("Cargando Banca_M -> Models/Banca.obj ...\n");
    Banca_M = Model(); Banca_M.LoadModel("Models/Banca.obj");
    printf("OK -> Banca_M\n");

    printf("Cargando BloquePasto_M -> Models/Bloquepasto.obj ...\n");
    BloquePasto_M = Model(); BloquePasto_M.LoadModel("Models/Bloquepasto.obj");
    printf("OK -> BloquePasto_M\n");

    printf("Cargando BasePlanetas_M -> Models/Baseplanetas.obj ...\n");
    BasePlanetas_M = Model(); BasePlanetas_M.LoadModel("Models/Baseplanetas.obj");
    printf("OK -> BasePlanetas_M\n");

    printf("Cargando RelojBase_M -> Models/relojbase.obj ...\n");
    RelojBase_M = Model(); RelojBase_M.LoadModel("Models/relojbase.obj");
    printf("OK -> RelojBase_M\n");

    printf("Cargando RelojEngrane1_M -> Models/relojengrane1.obj ...\n");
    RelojEngrane1_M = Model(); RelojEngrane1_M.LoadModel("Models/relojengrane1.obj");
    printf("OK -> RelojEngrane1_M\n");

    printf("Cargando RelojEngrane2_M -> Models/relojengrane2.obj ...\n");
    RelojEngrane2_M = Model(); RelojEngrane2_M.LoadModel("Models/relojengrane2.obj");
    printf("OK -> RelojEngrane2_M\n");

    printf("Cargando RelojEngrane3_M -> Models/relojengrane3.obj ...\n");
    RelojEngrane3_M = Model(); RelojEngrane3_M.LoadModel("Models/relojengrane3.obj");
    printf("OK -> RelojEngrane3_M\n");

    printf("Cargando RelojEngrane4_M -> Models/relojengrane4.obj ...\n");
    RelojEngrane4_M = Model(); RelojEngrane4_M.LoadModel("Models/relojengrane4.obj");
    printf("OK -> RelojEngrane4_M\n");

    printf("Cargando RelojGalRedE_M -> Models/relojgalrede.obj ...\n");
    RelojGalRedE_M = Model(); RelojGalRedE_M.LoadModel("Models/relojgalrede.obj");
    printf("OK -> RelojGalRedE_M\n");

    printf("Cargando RelojPlanetas_M -> Models/relojplanetas.obj ...\n");
    RelojPlanetas_M = Model(); RelojPlanetas_M.LoadModel("Models/relojplanetas.obj");
    printf("OK -> RelojPlanetas_M\n");

    printf("\n================ AVATARES - BIOSHOCK ================\n");

    printf("Cargando BigDaddyCuerpo_M -> Models/Cuerpobig.obj ...\n");
    BigDaddyCuerpo_M = Model(); BigDaddyCuerpo_M.LoadModel("Models/Cuerpobig.obj");
    printf("OK -> BigDaddyCuerpo_M\n");

    printf("Cargando BigDaddyTaladro_M -> Models/taladro.obj ...\n");
    BigDaddyTaladro_M = Model(); BigDaddyTaladro_M.LoadModel("Models/taladro.obj");
    printf("OK -> BigDaddyTaladro_M\n");

    printf("Cargando BigDaddyBrazoIzq_M -> Models/brazoizq.obj ...\n");
    BigDaddyBrazoIzq_M = Model(); BigDaddyBrazoIzq_M.LoadModel("Models/brazoizq.obj");
    printf("OK -> BigDaddyBrazoIzq_M\n");

    printf("Cargando BigDaddyBrazoDer_M -> Models/brazoder.obj ...\n");
    BigDaddyBrazoDer_M = Model(); BigDaddyBrazoDer_M.LoadModel("Models/brazoder.obj");
    printf("OK -> BigDaddyBrazoDer_M\n");

    printf("Cargando BigDaddyPiernaIzq_M -> Models/piernaizq.obj ...\n");
    BigDaddyPiernaIzq_M = Model(); BigDaddyPiernaIzq_M.LoadModel("Models/piernaizq.obj");
    printf("OK -> BigDaddyPiernaIzq_M\n");

    printf("Cargando BigDaddyPiernaDer_M -> Models/piernader.obj ...\n");
    BigDaddyPiernaDer_M = Model(); BigDaddyPiernaDer_M.LoadModel("Models/piernader.obj");
    printf("OK -> BigDaddyPiernaDer_M\n");

    printf("\n================ AVATARES - HARRY POTTER ================\n");

    printf("Cargando Hermione_HP_M -> Models/HermioneCompleta.obj ...\n");
    Hermione_HP_M = Model(); Hermione_HP_M.LoadModel("Models/HermioneCompleta.obj");
    printf("OK -> Hermione_HP_M\n");

    printf("Cargando BrazoDer_HP_M -> Models/brazoderhermione.obj ...\n");
    BrazoDer_HP_M = Model(); BrazoDer_HP_M.LoadModel("Models/brazoderhermione.obj");
    printf("OK -> BrazoDer_HP_M\n");

    printf("Cargando BrazoIzq_HP_M -> Models/brazoizqhermione.obj ...\n");
    BrazoIzq_HP_M = Model(); BrazoIzq_HP_M.LoadModel("Models/brazoizqhermione.obj");
    printf("OK -> BrazoIzq_HP_M\n");

    printf("Cargando PiernaDer_HP_M -> Models/piernaderHermione.obj ...\n");
    PiernaDer_HP_M = Model(); PiernaDer_HP_M.LoadModel("Models/piernaderHermione.obj");
    printf("OK -> PiernaDer_HP_M\n");

    printf("Cargando PiernaIzq_HP_M -> Models/piernaizqHermione.obj ...\n");
    PiernaIzq_HP_M = Model(); PiernaIzq_HP_M.LoadModel("Models/piernaizqHermione.obj");
    printf("OK -> PiernaIzq_HP_M\n");

    printf("\n================ AVATARES - CRASH ================\n");

    printf("Cargando Crash_Cuerpo_M -> Models/crashCharacterCuerpo.obj ...\n");
    Crash_Cuerpo_M = Model(); Crash_Cuerpo_M.LoadModel("Models/crashCharacterCuerpo.obj");
    printf("OK -> Crash_Cuerpo_M\n");

    printf("Cargando Crash_BrazoDer_M -> Models/crashCharacterBrazoDerecho.obj ...\n");
    Crash_BrazoDer_M = Model(); Crash_BrazoDer_M.LoadModel("Models/crashCharacterBrazoDerecho.obj");
    printf("OK -> Crash_BrazoDer_M\n");

    printf("Cargando Crash_BrazoIzq_M -> Models/crashCharacterBrazoIzquierdo.obj ...\n");
    Crash_BrazoIzq_M = Model(); Crash_BrazoIzq_M.LoadModel("Models/crashCharacterBrazoIzquierdo.obj");
    printf("OK -> Crash_BrazoIzq_M\n");

    printf("Cargando Crash_PiernaDer_M -> Models/crashCharacterPiernaDerecha.obj ...\n");
    Crash_PiernaDer_M = Model(); Crash_PiernaDer_M.LoadModel("Models/crashCharacterPiernaDerecha.obj");
    printf("OK -> Crash_PiernaDer_M\n");

    printf("Cargando Crash_PiernaIzq_M -> Models/crashCharacterPiernaIzquierda.obj ...\n");
    Crash_PiernaIzq_M = Model(); Crash_PiernaIzq_M.LoadModel("Models/crashCharacterPiernaIzquierda.obj");
    printf("OK -> Crash_PiernaIzq_M\n");

    printf("\n================ OBJETOS - BIOSHOCK ================\n");

    printf("Cargando BolsaDer_M -> Models/bolsader.obj ...\n");
    BolsaDer_M = Model(); BolsaDer_M.LoadModel("Models/bolsader.obj");
    printf("OK -> BolsaDer_M\n");

    printf("Cargando BolsaIzq_M -> Models/bolsaizq.obj ...\n");
    BolsaIzq_M = Model(); BolsaIzq_M.LoadModel("Models/bolsaizq.obj");
    printf("OK -> BolsaIzq_M\n");

    printf("Cargando BolsaMid_M -> Models/bolsamid.obj ...\n");
    BolsaMid_M = Model(); BolsaMid_M.LoadModel("Models/bolsamid.obj");
    printf("OK -> BolsaMid_M\n");

    printf("Cargando Plasmido_M -> Models/plasmido.obj ...\n");
    Plasmido_M = Model(); Plasmido_M.LoadModel("Models/plasmido.obj");
    printf("OK -> Plasmido_M\n");

    printf("Cargando Radio_M -> Models/Radio.obj ...\n");
    Radio_M = Model(); Radio_M.LoadModel("Models/Radio.obj");
    printf("OK -> Radio_M\n");

    printf("\n================ OBJETOS - HARRY POTTER ================\n");

    printf("Cargando Carro_HP_M -> Models/CarroCompleto.obj ...\n");
    Carro_HP_M = Model(); Carro_HP_M.LoadModel("Models/CarroCompleto.obj");
    printf("OK -> Carro_HP_M\n");

    printf("Cargando LlantaFD_HP_M -> Models/llSuperiorDer.obj ...\n");
    LlantaFD_HP_M = Model(); LlantaFD_HP_M.LoadModel("Models/llSuperiorDer.obj");
    printf("OK -> LlantaFD_HP_M\n");

    printf("Cargando LlantaFI_HP_M -> Models/llSuperiorIzq.obj ...\n");
    LlantaFI_HP_M = Model(); LlantaFI_HP_M.LoadModel("Models/llSuperiorIzq.obj");
    printf("OK -> LlantaFI_HP_M\n");

    printf("Cargando LlantaAD_HP_M -> Models/llAtrasDer.obj ...\n");
    LlantaAD_HP_M = Model(); LlantaAD_HP_M.LoadModel("Models/llAtrasDer.obj");
    printf("OK -> LlantaAD_HP_M\n");

    printf("Cargando LlantaAI_HP_M -> Models/llAtrasIzq.obj ...\n");
    LlantaAI_HP_M = Model(); LlantaAI_HP_M.LoadModel("Models/llAtrasIzq.obj");
    printf("OK -> LlantaAI_HP_M\n");

    printf("Cargando Bolsa_HP_M -> Models/bolsa.obj ...\n");
    Bolsa_HP_M = Model(); Bolsa_HP_M.LoadModel("Models/bolsa.obj");
    printf("OK -> Bolsa_HP_M\n");

    printf("Cargando Giratiempo_HP_M -> Models/giratiempo.obj ...\n");
    Giratiempo_HP_M = Model(); Giratiempo_HP_M.LoadModel("Models/giratiempo.obj");
    printf("OK -> Giratiempo_HP_M\n");

    printf("Cargando LibroHechizos_HP_M -> Models/libroHechizos.obj ...\n");
    LibroHechizos_HP_M = Model(); LibroHechizos_HP_M.LoadModel("Models/libroHechizos.obj");
    printf("OK -> LibroHechizos_HP_M\n");

    printf("Cargando Snitch_HP_M -> Models/snitch.obj ...\n");
    Snitch_HP_M = Model(); Snitch_HP_M.LoadModel("Models/snitch.obj");
    printf("OK -> Snitch_HP_M\n");

    printf("Cargando Copa_HP_M -> Models/copa.obj ...\n");
    Copa_HP_M = Model(); Copa_HP_M.LoadModel("Models/copa.obj");
    printf("OK -> Copa_HP_M\n");

    printf("Cargando ExpresoHowarts_M -> Models/expressHowartsCompleto.obj ...\n");
    ExpresoHowarts_M = Model(); ExpresoHowarts_M.LoadModel("Models/expressHowartsCompleto.obj");
    printf("OK -> ExpresoHowarts_M\n");

    printf("Cargando LlantaSteamCar3_M -> Models/llantaSteamCar3BikeHarry.obj ...\n");
    LlantaSteamCar3_M = Model(); LlantaSteamCar3_M.LoadModel("Models/llantaSteamCar3BikeHarry.obj");
    printf("OK -> LlantaSteamCar3_M\n");

    printf("Cargando SteamCar3_M -> Models/steamCar3BikeHarry.obj ...\n");
    SteamCar3_M = Model(); SteamCar3_M.LoadModel("Models/steamCar3BikeHarry.obj");
    printf("OK -> SteamCar3_M\n");

    printf("\n================ OBJETOS - CRASH ================\n");

    printf("Cargando Crash_GoKart_M -> Models/crashOnGoKart.obj ...\n");
    Crash_GoKart_M = Model(); Crash_GoKart_M.LoadModel("Models/crashOnGoKart.obj");
    printf("OK -> Crash_GoKart_M\n");

    printf("Cargando LlantaFD_Crash_M -> Models/ruedaSteamCar1CrashGrunt.obj ...\n");
    LlantaFD_Crash_M = Model(); LlantaFD_Crash_M.LoadModel("Models/ruedaSteamCar1CrashGrunt.obj");
    printf("OK -> LlantaFD_Crash_M\n");

    printf("Cargando LlantaFI_Crash_M -> Models/ruedaSteamCar1CrashGrunt.obj ...\n");
    LlantaFI_Crash_M = Model(); LlantaFI_Crash_M.LoadModel("Models/ruedaSteamCar1CrashGrunt.obj");
    printf("OK -> LlantaFI_Crash_M\n");

    printf("Cargando LlantaAD_Crash_M -> Models/ruedaSteamCar1CrashGrunt.obj ...\n");
    LlantaAD_Crash_M = Model(); LlantaAD_Crash_M.LoadModel("Models/ruedaSteamCar1CrashGrunt.obj");
    printf("OK -> LlantaAD_Crash_M\n");

    printf("Cargando LlantaAI_Crash_M -> Models/ruedaSteamCar1CrashGrunt.obj ...\n");
    LlantaAI_Crash_M = Model(); LlantaAI_Crash_M.LoadModel("Models/ruedaSteamCar1CrashGrunt.obj");
    printf("OK -> LlantaAI_Crash_M\n");

    printf("Cargando SteamCar_Crash_M -> Models/steamCar.obj ...\n");
    SteamCar_Crash_M = Model(); SteamCar_Crash_M.LoadModel("Models/steamCar.obj");
    printf("OK -> SteamCar_Crash_M\n");

    printf("Cargando ArbolWumpa_M -> Models/arbolWumpa.obj ...\n");
    ArbolWumpa_M = Model(); ArbolWumpa_M.LoadModel("Models/arbolWumpa.obj");
    printf("OK -> ArbolWumpa_M\n");

    printf("Cargando BaseCilindro_M -> Models/baseCilindroCrash.obj ...\n");
    BaseCilindro_M = Model(); BaseCilindro_M.LoadModel("Models/baseCilindroCrash.obj");
    printf("OK -> BaseCilindro_M\n");

    printf("Cargando BolaRoca_M -> Models/bolaRocaCrash.obj ...\n");
    BolaRoca_M = Model(); BolaRoca_M.LoadModel("Models/bolaRocaCrash.obj");
    printf("OK -> BolaRoca_M\n");

    printf("Cargando CajaAku_M -> Models/cajaAku.obj ...\n");
    CajaAku_M = Model(); CajaAku_M.LoadModel("Models/cajaAku.obj");
    printf("OK -> CajaAku_M\n");

    printf("Cargando CajaCaraCrash_M -> Models/cajaCaraCrash.obj ...\n");
    CajaCaraCrash_M = Model(); CajaCaraCrash_M.LoadModel("Models/cajaCaraCrash.obj");
    printf("OK -> CajaCaraCrash_M\n");

    printf("Cargando CajaCheckPoint_M -> Models/cajaCheckPoint.obj ...\n");
    CajaCheckPoint_M = Model(); CajaCheckPoint_M.LoadModel("Models/cajaCheckPoint.obj");
    printf("OK -> CajaCheckPoint_M\n");

    printf("Cargando CajaFlechaArriba_M -> Models/cajaFlechaArriba.obj ...\n");
    CajaFlechaArriba_M = Model(); CajaFlechaArriba_M.LoadModel("Models/cajaFlechaArriba.obj");
    printf("OK -> CajaFlechaArriba_M\n");

    printf("Cargando CajaMetalExclamation_M -> Models/cajaMetalExclamation.obj ...\n");
    CajaMetalExclamation_M = Model(); CajaMetalExclamation_M.LoadModel("Models/cajaMetalExclamation.obj");
    printf("OK -> CajaMetalExclamation_M\n");

    printf("Cargando CajaNitro_M -> Models/cajaNitro.obj ...\n");
    CajaNitro_M = Model(); CajaNitro_M.LoadModel("Models/cajaNitro.obj");
    printf("OK -> CajaNitro_M\n");

    printf("Cargando CajaNitroExclamation_M -> Models/cajaNitroExclamation.obj ...\n");
    CajaNitroExclamation_M = Model(); CajaNitroExclamation_M.LoadModel("Models/cajaNitroExclamation.obj");
    printf("OK -> CajaNitroExclamation_M\n");

    printf("Cargando CajaNormalCrash_M -> Models/cajaNormalCrash.obj ...\n");
    CajaNormalCrash_M = Model(); CajaNormalCrash_M.LoadModel("Models/cajaNormalCrash.obj");
    printf("OK -> CajaNormalCrash_M\n");

    printf("Cargando CajaQuestion_M -> Models/cajaQuestion.obj ...\n");
    CajaQuestion_M = Model(); CajaQuestion_M.LoadModel("Models/cajaQuestion.obj");
    printf("OK -> CajaQuestion_M\n");

    printf("Cargando CajaTNT_M -> Models/cajaTNT.obj ...\n");
    CajaTNT_M = Model(); CajaTNT_M.LoadModel("Models/cajaTNT.obj");
    printf("OK -> CajaTNT_M\n");

    printf("Cargando CocoOnGoKart_M -> Models/cocoOnGoKart.obj ...\n");
    CocoOnGoKart_M = Model(); CocoOnGoKart_M.LoadModel("Models/cocoOnGoKart.obj");
    printf("OK -> CocoOnGoKart_M\n");

    printf("Cargando CortexOnGoKart_M -> Models/cortexOnGoKart.obj ...\n");
    CortexOnGoKart_M = Model(); CortexOnGoKart_M.LoadModel("Models/cortexOnGoKart.obj");
    printf("OK -> CortexOnGoKart_M\n");

    printf("Cargando CrystalCrash_M -> Models/crystalCrash.obj ...\n");
    CrystalCrash_M = Model(); CrystalCrash_M.LoadModel("Models/crystalCrash.obj");
    printf("OK -> CrystalCrash_M\n");

    printf("Cargando CrystalHielo_M -> Models/crystalHielo.obj ...\n");
    CrystalHielo_M = Model(); CrystalHielo_M.LoadModel("Models/crystalHielo.obj");
    printf("OK -> CrystalHielo_M\n");

    printf("Cargando FrutaWumpa_M -> Models/frutaWumpa.obj ...\n");
    FrutaWumpa_M = Model(); FrutaWumpa_M.LoadModel("Models/frutaWumpa.obj");
    printf("OK -> FrutaWumpa_M\n");

    printf("Cargando JoyasCrash_M -> Models/joyasCrash.obj ...\n");
    JoyasCrash_M = Model(); JoyasCrash_M.LoadModel("Models/joyasCrash.obj");
    printf("OK -> JoyasCrash_M\n");

    printf("Cargando MascaraAku_M -> Models/mascaraAku.obj ...\n");
    MascaraAku_M = Model(); MascaraAku_M.LoadModel("Models/mascaraAku.obj");
    printf("OK -> MascaraAku_M\n");

    printf("Cargando MascaraAkuNoche_M -> Models/mascaraAkuNoche.obj ...\n");
    MascaraAkuNoche_M = Model(); MascaraAkuNoche_M.LoadModel("Models/mascaraAkuNoche.obj");
    printf("OK -> MascaraAkuNoche_M\n");

    printf("Cargando MascaraUka_M -> Models/mascaraUka.obj ...\n");
    MascaraUka_M = Model(); MascaraUka_M.LoadModel("Models/mascaraUka.obj");
    printf("OK -> MascaraUka_M\n");

    printf("Cargando Pista_M -> Models/pista.obj ...\n");
    Pista_M = Model(); Pista_M.LoadModel("Models/pista.obj");
    printf("OK -> Pista_M\n");

    printf("Cargando SteamCar1_M -> Models/steamCar1CrashandGrunt.obj ...\n");
    SteamCar1_M = Model(); SteamCar1_M.LoadModel("Models/steamCar1CrashandGrunt.obj");
    printf("OK -> SteamCar1_M\n");

    printf("Cargando SteamCar2_M -> Models/steamCar2ChiefCoco.obj ...\n");
    SteamCar2_M = Model(); SteamCar2_M.LoadModel("Models/steamCar2ChiefCoco.obj");
    printf("OK -> SteamCar2_M\n");

    printf("Cargando Totem1Crash_M -> Models/totem1Crash.obj ...\n");
    Totem1Crash_M = Model(); Totem1Crash_M.LoadModel("Models/totem1Crash.obj");
    printf("OK -> Totem1Crash_M\n");

    printf("Cargando Totem2Crash_M -> Models/totem2Crash.obj ...\n");
    Totem2Crash_M = Model(); Totem2Crash_M.LoadModel("Models/totem2Crash.obj");
    printf("OK -> Totem2Crash_M\n");

    printf("Cargando RuedaSteamCar2_M -> Models/ruedaSteamCar2ChiefCoco.obj ...\n");
    RuedaSteamCar2_M = Model(); RuedaSteamCar2_M.LoadModel("Models/ruedaSteamCar2ChiefCoco.obj");
    printf("OK -> RuedaSteamCar2_M\n");


    printf("==== MODELOS CARGADOS ====\n\n");
    

    // ------------------------------------------------
    // MATERIALES
    // ------------------------------------------------
    Material_brillante = Material(4.0f, 256);
    Material_opaco = Material(0.3f, 4);

    // ------------------------------------------------
    // LUCES — inicialización estática
    // ------------------------------------------------
    mainLight = DirectionalLight(1.f, 1.f, 1.f, 0.3f, 0.6f, 0.f, -0.5f, 1.f);

    unsigned int pointLightCount = 0;

    // PL0 — Radio: naranja, solo de noche (posición actualizada cada frame)
    pointLights[0] = PointLight(1.f, 0.45f, 0.f, 0.f, 0.f, 105.31f, -1.5f, -67.54f, 0.5f, 0.2f, 0.1f);
    pointLightCount++;
    // PL1 — Plasmido: rojo, sigue la flotación (posición actualizada cada frame)
    pointLights[1] = PointLight(1.f, 0.f, 0.f, 0.f, 1.f, -10.f, -2.f, 0.f, 0.5f, 0.3f, 0.2f);
    pointLightCount++;
    // PL2 — Reservada (copa HP, desactivada)
    pointLights[2] = PointLight(0.53f, 0.81f, 0.98f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.3f, 0.2f, 0.1f);
    pointLightCount++;
    // PL3 — Big Daddy: amarillo, solo de noche (posición actualizada cada frame)
    pointLights[3] = PointLight(1.f, 0.85f, 0.2f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.3f, 0.1f, 0.05f);
    pointLightCount++;
    // PL4 — Hermione: blanco, siempre activo (posición actualizada cada frame)
    pointLights[4] = PointLight(1.f, 1.f, 1.f, 0.f, 0.8f, 0.f, 0.f, 0.f, 0.5f, 0.2f, 0.1f);
    pointLightCount++;
    // PL5 — Farola más cercana: amarillo suave, solo de noche (posición actualizada cada frame)
    pointLights[5] = PointLight(1.f, 0.93f, 0.72f, 0.f, 0.f, 3.f, 14.76f, 117.09f, 0.3f, 0.1f, 0.05f);
    pointLightCount++;

    unsigned int spotLightCount = 0;

    // SL0 — Linterna de cámara (flash, posición actualizada cada frame)
    spotLights[0] = SpotLight(1.f, 1.f, 1.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 0.f, 5.f);
    spotLightCount++;
    // SL1 — Desactivada (reservada)
    spotLights[1] = SpotLight(0.f, 0.f, 1.f, 0.f, 0.f, 5.f, 10.f, 0.f, 0.f, -5.f, 0.f, 1.f, 0.f, 0.f, 15.f);
    spotLightCount++;
    // SL2 — Foco derecho de farola (posición actualizada cada frame)
    spotLights[2] = SpotLight(1.f, 0.93f, 0.72f, 0.f, 0.f, 0.f, -100.f, 0.f, 0.f, -1.f, 0.f, 1.f, 0.04f, 0.01f, 40.f);
    spotLightCount++;
    // SL3 — Foco izquierdo de farola (posición actualizada cada frame)
    spotLights[3] = SpotLight(1.f, 0.93f, 0.72f, 0.f, 0.f, 0.f, -100.f, 0.f, 0.f, -1.f, 0.f, 1.f, 0.04f, 0.01f, 40.f);
    spotLightCount++;
    // SL4 — Linterna de Hermione, toggle con tecla F
    spotLights[4] = SpotLight(1.f, 1.f, 0.9f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 0.f, 20.f);
    spotLightCount++;
    // SL5 — Foco frontal del tren, solo de noche (posición actualizada cada frame)
    spotLights[5] = SpotLight(1.f, 0.95f, 0.6f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 15.f);
    spotLightCount++;

    // ------------------------------------------------
    // UNIFORMS
    // ------------------------------------------------
    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
    GLuint uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
    GLuint uniformTextureOffset = 0, uniformColor = 0;

    glm::mat4 projection = glm::perspective(45.f,
        (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(),
        0.1f, 1000.f);

    lastTime = glfwGetTime();

    // ------------------------------------------------
    // VARIABLES — ESCENARIO
    // ------------------------------------------------

    // Murallas
    float     escMurallaL = 1.0f;
    glm::vec3 scaleMurallaL = glm::vec3(1.f);
    glm::vec3 posMurallaL_1 = glm::vec3(-200.f, -2.f, -150.f);
    glm::vec3 rotMurallaL_1 = glm::vec3(0.f);
    glm::vec3 posMurallaL_2 = glm::vec3(200.f, -2.f, 150.f);
    glm::vec3 rotMurallaL_2 = glm::vec3(0.f, 180.f, 0.f);

    // Farola (objeto padre para SpotLights; el array de instancias se define en render)
    float     escFarola = 1.0f;
    glm::vec3 posFarola = glm::vec3(3.f, -2.f, 117.09f);
    glm::vec3 rotFarola = glm::vec3(0.f);
    glm::vec3 offsetPaloFarola = glm::vec3(0.f);
    glm::vec3 rotPaloFarola = glm::vec3(0.f);
    glm::vec3 scalePaloFarola = glm::vec3(1.f);
    glm::vec3 offsetFocoDerFarola = glm::vec3(0.f, 8.38f, 3.05f);
    glm::vec3 rotFocoDerFarola = glm::vec3(0.f);
    glm::vec3 scaleFocoDerFarola = glm::vec3(1.f);
    glm::vec3 offsetFocoIzqFarola = glm::vec3(0.f, 8.38f, -3.0f);
    glm::vec3 rotFocoIzqFarola = glm::vec3(0.f);
    glm::vec3 scaleFocoIzqFarola = glm::vec3(1.f);
    // Posiciones locales de luz dentro de cada foco (escala doble aplicada en render)
    glm::vec3 luzLocalFocoDer = glm::vec3(0.f, 16.76f, 6.10f);
    glm::vec3 luzLocalFocoIzq = glm::vec3(0.f, 16.76f, -6.0f);
    glm::vec3 dirLocalFocoDer = glm::normalize(glm::vec3(0.f, -1.f, 0.f));
    glm::vec3 dirLocalFocoIzq = glm::normalize(glm::vec3(0.f, -1.f, 0.f));

    // Bloques de pasto distribuidos en anillo
    float     escBloquePasto = 1.0f;
    glm::vec3 scaleBloquePasto = glm::vec3(1.f);
    glm::vec3 pastoBasePos[4] = {
        {160.f,-2.f,115.f},{-110.f,-2.f,170.f},
        {108.f,-2.f,-173.f},{-160.f,-2.f,-120.f}
    };
    float pastoRotY[4] = { 0.f,-90.f,90.f,180.f };
    glm::vec3 pastoPos[12]; glm::vec3 pastoRot[12];
    for (int k = 0; k < 4; k++) {
        pastoPos[k] = pastoBasePos[k];
        pastoRot[k] = glm::vec3(0.f, pastoRotY[k], 0.f);
    }
    for (int k = 0; k < 4; k++) {
        pastoPos[k + 8] = MoveTowardsOriginXZ(pastoBasePos[k], 50.f);
        pastoRot[k + 8] = glm::vec3(0.f, pastoRotY[k], 0.f);
    }

    // Reloj de planetas (engranes con órbita continua)
    float     escReloj = 4.0f;
    glm::vec3 posReloj = glm::vec3(0.f, -2.f, -20.f);
    glm::vec3 rotReloj = glm::vec3(0.f);
    glm::vec3 offsetBasePlanetas = glm::vec3(0.f);     glm::vec3 rotBasePlanetas = glm::vec3(0.f); glm::vec3 scaleBasePlanetas = glm::vec3(1.f);
    glm::vec3 offsetRelojBase = glm::vec3(0.f);     glm::vec3 rotRelojBase = glm::vec3(0.f); glm::vec3 scaleRelojBase = glm::vec3(1.f);
    glm::vec3 offsetRelojEngrane1 = glm::vec3(0.f, .3f, .3f); glm::vec3 rotRelojEngrane1 = glm::vec3(0.f); glm::vec3 scaleRelojEngrane1 = glm::vec3(1.f);
    glm::vec3 offsetRelojEngrane2 = glm::vec3(0.f, .3f, 0.f); glm::vec3 rotRelojEngrane2 = glm::vec3(0.f); glm::vec3 scaleRelojEngrane2 = glm::vec3(1.f);
    glm::vec3 offsetRelojEngrane3 = glm::vec3(0.f, .1f, 0.f); glm::vec3 rotRelojEngrane3 = glm::vec3(0.f); glm::vec3 scaleRelojEngrane3 = glm::vec3(1.f);
    glm::vec3 offsetRelojEngrane4 = glm::vec3(0.f, .1f, 0.f); glm::vec3 rotRelojEngrane4 = glm::vec3(0.f); glm::vec3 scaleRelojEngrane4 = glm::vec3(1.f);
    glm::vec3 offsetRelojGalRedE = glm::vec3(0.f);     glm::vec3 rotRelojGalRedE = glm::vec3(0.f); glm::vec3 scaleRelojGalRedE = glm::vec3(1.f);
    glm::vec3 offsetRelojPlanetas = glm::vec3(.1f, 2.2f, -.1f); glm::vec3 rotRelojPlanetas = glm::vec3(0.f); glm::vec3 scaleRelojPlanetas = glm::vec3(1.f);
    float velE1 = 15.f, velE2 = 6.f, velE3 = 6.f, velE4 = 6.f, velGR = 6.f, velPL = 6.f;
    float angE1 = 0.f, angE2 = 0.f, angE3 = 0.f, angE4 = 0.f, angGR = 0.f, angPL = 0.f;

    // ------------------------------------------------
    // VARIABLES — CICLO DÍA/NOCHE
    // Duración total: 45s día + 15s transición + 45s noche + 15s amanecer = 120s (2 min)
    // ------------------------------------------------
    float duracionDia = 45.0f;
    float duracionTrans = 15.0f;
    int   estadoCiclo = 0;   // 0=día, 1=atardecer, 2=noche, 3=amanecer
    float ciclTimer = 0.0f;
    bool  prevTeclaT = false;
    float debugTimer = 0.0f;

    // ------------------------------------------------
    // VARIABLES — AVATAR BIG DADDY (BioShock)
    // ------------------------------------------------
    float     escBigDaddy = 2.0f;
    glm::vec3 posBigDaddy = glm::vec3(0.f, -1.f, 0.f);
    glm::vec3 offsetCuerpo = glm::vec3(-0.9f, 4.f, 1.f);   glm::vec3 rotCuerpo = glm::vec3(0.f);
    glm::vec3 offsetBrazoIzq = glm::vec3(-2.f, 4.2f, 0.f);   glm::vec3 rotBrazoIzq = glm::vec3(0.f);
    glm::vec3 offsetBrazoDer = glm::vec3(0.2f, 4.2f, 0.f);   glm::vec3 rotBrazoDer = glm::vec3(0.f);
    glm::vec3 offsetPiernaIzq = glm::vec3(-1.5f, 2.7f, 0.f);  glm::vec3 rotPiernaIzq = glm::vec3(0.f);
    glm::vec3 offsetPiernaDer = glm::vec3(-0.15f, 2.7f, 0.f); glm::vec3 rotPiernaDer = glm::vec3(0.f);
    glm::vec3 offsetTaladro = glm::vec3(1.6f, 0.7f, 0.1f);  glm::vec3 rotTaladro = glm::vec3(0.f, 0.f, 45.f);
    float taladroRot = 1.0f;
    float caminataTime = 0.f, caminataSpeed = 3.f, caminataAmp = 25.f, caminataBrazoAmp = 10.f;
    bool  bigDaddyCaminando = true;

    glm::vec3 bigDaddyPosActual = glm::vec3(-100.31f, posBigDaddy.y, 108.96f);
    float bigDaddyRotY = 0.f;
    float bigDaddyVel = 9.f;

    // Ruta cerrada por la que patrulla el Big Daddy
    std::vector<glm::vec3> rutaBigDaddy = {
        {-100.31f,posBigDaddy.y,108.96f}, {-101.90f,posBigDaddy.y,-113.97f},
        {-94.94f, posBigDaddy.y,-113.87f},{  8.21f, posBigDaddy.y,-113.42f},
        {  1.95f, posBigDaddy.y, -62.86f},{ 25.05f, posBigDaddy.y, -66.81f},
        { 102.11f,posBigDaddy.y,-117.19f},{ 104.51f,posBigDaddy.y,-103.92f},
        { 102.80f,posBigDaddy.y,  -6.70f},{ 71.21f, posBigDaddy.y,   7.65f},
        {  98.84f,posBigDaddy.y,  64.48f},{ 104.10f,posBigDaddy.y, 108.58f},
        {  61.31f,posBigDaddy.y, 106.53f},{-17.02f, posBigDaddy.y, 109.65f},
        { -17.15f,posBigDaddy.y,  65.64f},{-49.46f, posBigDaddy.y,   3.16f},
        {  56.64f,posBigDaddy.y,  -0.67f},{-25.46f, posBigDaddy.y,  70.07f},
        {-101.35f,posBigDaddy.y, 112.95f}
    };
    int bigDaddyNodoActual = 1;
    float vibracionAmp = 0.05f, vibracionRangoMax = 15.f;

    // ------------------------------------------------
    // VARIABLES — OBJETOS BIOSHOCK
    // ------------------------------------------------
    float     escMedkit = 0.25f;
    glm::vec3 posMedkit = glm::vec3(-60.18f, 0.71f, 113.02f);
    glm::vec3 offsetBolsaDer = glm::vec3(0.f, 0.2f, -0.13f);
    glm::vec3 offsetBolsaIzq = glm::vec3(0.f, 0.f, 1.1f);
    float puertaAngulo = 0.f, puertaTarget = 0.f, puertaSpeed = 3.f, distActivacionMedkit = 5.f;

    float     escPlasmido = 3.0f;
    glm::vec3 posPlasmido = glm::vec3(70.20f, 20.44f, 80.32f);
    float plasTime = 0.f, plasFloatSpeed = 0.25f, plasFloatAmp = 0.04f;
    float plasRotSpeed = 3.f, plasRotY = 0.f, plasHeartSpeed = 0.4f, plasHeartAmp = 0.25f;

    // ------------------------------------------------
    // VARIABLES — AVATAR HERMIONE (Harry Potter)
    // ------------------------------------------------
    glm::vec3 hpOffsetEscena = glm::vec3(55.f, 0.f, 25.f);
    float hpPosX = 0.f, hpPosZ = 0.f, hpRotPersonaje = 0.f, hpVelocidad = 30.f;
    float hpRotBrazoDer = 0.f, hpRotBrazoIzq = 0.f, hpRotPiernaDer = 0.f, hpRotPiernaIzq = 0.f;

    // ------------------------------------------------
    // VARIABLES — OBJETOS HARRY POTTER
    // ------------------------------------------------
    glm::vec3 hpPosGiratiempo = glm::vec3(-0.17f, 14.74f, -20.17f);

    glm::vec3 hpPosLibro = glm::vec3(-44.98f, 0.90f, -116.52f);
    glm::vec3 hpPosCopa = glm::vec3(200.62f, 13.34f, -97.50f);

    // Copa (animación continua de flotación + latido + rotación)
    float copaTime = 0.f, copaFloatSpeed = 0.25f, copaFloatAmp = 0.04f;
    float copaRotSpeed = 3.f, copaRotY = 0.f, copaHeartSpeed = 0.4f, copaHeartAmp = 0.20f;

    // Giratiempo — animación por keyframes (tecla 4)
    std::vector<glm::vec3> gtKeyframes = {
        hpPosGiratiempo,
        hpPosGiratiempo + glm::vec3(0.f,1.5f,0.f),
        hpPosGiratiempo + glm::vec3(0.8f,1.8f,0.f),
        hpPosGiratiempo
    };
    bool      gtAnimActiva = false;
    int       gtNodoActual = 0;
    glm::vec3 gtPosActual = hpPosGiratiempo;
    float     gtRotY = 0.f;
    float     gtVelocidad = 2.0f;
    bool      prevTeclaGT = false;

    // Snitch — ruta automática (tecla 3)
    std::vector<glm::vec3> hpSnitchRuta = {
        { 99.43f, 5.88f,-32.88f},{ 49.79f, 3.06f, 47.08f},
        { 31.90f, 5.82f,102.71f},{-11.96f, 2.07f, 19.60f},
        {-11.74f, 3.85f,-27.56f},{ 17.69f, 4.55f,-103.31f},
        {-97.00f, 3.90f,-107.43f},{-98.47f, 6.74f, -5.85f},
        {-99.56f, 4.43f, 85.56f},{ 33.56f, 4.25f,  4.02f}
    };
    glm::vec3 hpPosSnitch = hpSnitchRuta[0];
    bool  hpSnitchVisible = true, hpSnitchAnimActiva = false;
    bool  prevTeclaAuto3 = false;
    int   hpSnitchWP = 1;
    float hpSnitchVel = 15.f;
    float hpRotCarro = 0.f;   // reutilizado para UpdatePathFollower de la snitch

    // ------------------------------------------------
    // VARIABLES — PISTA Y CARROS CRASH
    // ------------------------------------------------
    glm::vec3 posPista = glm::vec3(0.f, -1.5f, 20.f);

    // ------------------------------------------------
    // VARIABLES — TREN (Expreso de Hogwarts, keyframes)
    // Animación por keyframes: ruta triplicada sobre el circuito aéreo
    // Tecla KP_0 para arrancar/parar
    // ------------------------------------------------
    bool      trenAnimActiva = false;
    bool      prevTeclaTren = false;
    glm::vec3 trenPos = glm::vec3(0.f, 15.f, 0.f);
    float     trenRotY = 0.f;
    float     trenRotLlantas = 0.f;
    float     trenVelocidad = 18.f;
    int       trenNodoActual = 0;

    float pistoEscX = 3.4f, pistoEscZ = 4.0f;
    float alturaVuelo = 35.0f;

    // Construcción de la ruta triplicada (3 puntos por segmento para curvas suaves)
    std::vector<glm::vec3> rutaTren;
    for (int k = 0; k < (int)puntosPista.size(); k++) {
        int kSig = (k + 1) % (int)puntosPista.size();
        glm::vec3 pA = puntosPista[k];
        glm::vec3 pB = puntosPista[kSig];
        rutaTren.push_back({ posPista.x + pA.x * pistoEscX, alturaVuelo, posPista.z + pA.y * pistoEscZ });
        glm::vec3 p13 = pA + (pB - pA) * (1.f / 3.f);
        rutaTren.push_back({ posPista.x + p13.x * pistoEscX, alturaVuelo, posPista.z + p13.y * pistoEscZ });
        glm::vec3 p23 = pA + (pB - pA) * (2.f / 3.f);
        rutaTren.push_back({ posPista.x + p23.x * pistoEscX, alturaVuelo, posPista.z + p23.y * pistoEscZ });
    }
    trenPos = rutaTren[0];

    // ------------------------------------------------
    // VARIABLES — RADIO
    // ------------------------------------------------
    glm::vec3 posRadio = glm::vec3(105.31f, -1.5f, -67.54f);
    bool radioEncendido = false;
    bool prevTeclaRadio = false;
    float distActivacionRadio = 6.0f;

    // ------------------------------------------------
    // VARIABLES — SISTEMA DE CÁMARAS
    // Tecla ,   → modo 1: 3ra persona ligada a Hermione
    // Tecla .   → modo 2: aérea desplazable WASD
    // Tecla M   → modo 3: libre WASD + mouse
    // Tecla N   → modo 4: puntos de interés (3 posiciones fijas capturadas)
    // ------------------------------------------------
    int  modoCamara = 1;
    bool prevTeclaVirgula = false;
    bool prevTeclaPunto = false;
    bool prevTeclaM = false;
    bool prevTeclaN = false;

    // Parámetros 3ra persona
    float hpCamDistTP = 6.0f;
    float hpCamAlturaTP = 3.0f;
    float hpCamLookAhead = 1.5f;
    float hpCamLookY = 1.2f;

    // Parámetros cámara aérea
    glm::vec3 hpCamAereaPos(0.f);
    float hpCamAereaVel = 12.0f;
    float hpCamAereaAltura = 16.0f;
    hpCamAereaPos = hpOffsetEscena + glm::vec3(0.f, hpCamAereaAltura, 18.f);

    // Tres puntos de interés fijos capturados en escena
    struct PuntoInteres { glm::vec3 pos; glm::vec3 dir; };
    PuntoInteres puntosInteres[3] = {
        { glm::vec3(-127.07f,6.53f, 2.11f), glm::normalize(glm::vec3(-0.999f,-0.026f,-0.042f)) },
        { glm::vec3(-7.02f,3.30f, 0.40f), glm::normalize(glm::vec3(0.047f, 0.115f,-0.992f)) },
        { glm::vec3(141.71f,6.30f, 0.38f), glm::normalize(glm::vec3(1.000f, 0.021f, 0.005f)) }
    };
    int piActual = 0;

    // Spotlight Hermione — toggle con F
    bool hermionaSpotActiva = false;
    bool prevTeclaF = false;

    // ------------------------------------------------
    // LOOP PRINCIPAL
    // ------------------------------------------------
    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        if (deltaTime > 0.05f) deltaTime = 0.05f;
        lastTime = now;

        glfwPollEvents();

        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        if (modoCamara == 3)
            camera.keyControl(mainWindow.getsKeys(), deltaTime);

        // Debug: posición e instrucciones de captura impresas en consola
        debugTimer += deltaTime;
        if (debugTimer >= 0.05f) {
            debugTimer = 0.0f;
            glm::vec3 pos = camera.getCameraPosition();
            glm::vec3 dir = camera.getCameraDirection();
            printf("\r  POS( x:% 7.2f  y:% 7.2f  z:% 7.2f )   DIR( x:% 5.2f  y:% 5.2f  z:% 5.2f )   [C=captura]  ",
                pos.x, pos.y, pos.z, dir.x, dir.y, dir.z);
            fflush(stdout);
        }

        // Sistema de captura de waypoints
        bool teclaCActual = mainWindow.getsKeys()[GLFW_KEY_C];
        if (teclaCActual && !teclaCAnterior) {
            modoCaptura = !modoCaptura;
            if (modoCaptura) {
                printf("\n--- CAPTURA ACTIVADA ---  P=guardar  L=limpiar  C=salir\n");
                capturaContador = 0; waypointsCapturados.clear();
            }
            else {
                printf("\n--- CAPTURA DESACTIVADA --- total: %d puntos\n", capturaContador);
            }
        }
        teclaCAnterior = teclaCActual;

        bool teclaPActual = mainWindow.getsKeys()[GLFW_KEY_P];
        if (teclaPActual && !teclaPAnterior && modoCaptura) {
            glm::vec3 cp = camera.getCameraPosition();
            glm::vec3 pt = { cp.x, cp.y - 1.5f, cp.z };
            waypointsCapturados.push_back(pt);
            printf("\n\tglm::vec3(%.2ff, %.2ff, %.2ff),  // %d\n", pt.x, pt.y, pt.z, capturaContador + 1);
            fflush(stdout); capturaContador++;
        }
        teclaPAnterior = teclaPActual;

        bool teclaLActual = mainWindow.getsKeys()[GLFW_KEY_L];
        if (teclaLActual && !teclaLAnterior && modoCaptura) {
            printf("\nLista limpiada\n");
            capturaContador = 0; waypointsCapturados.clear();
        }
        teclaLAnterior = teclaLActual;

        bool* keys = mainWindow.getsKeys();

        // ============================================
        // CONTROLES DE CÁMARA
        // ============================================
        bool currVirgula = keys[GLFW_KEY_COMMA];
        bool currPunto = keys[GLFW_KEY_PERIOD];
        bool currM = keys[GLFW_KEY_M];
        bool currN = keys[GLFW_KEY_N];

        if (currVirgula && !prevTeclaVirgula) { modoCamara = 1; printf("Camara: 3ra persona\n"); }
        if (currPunto && !prevTeclaPunto) { modoCamara = 2; printf("Camara: aerea\n"); }
        if (currM && !prevTeclaM) { modoCamara = 3; printf("Camara: libre\n"); }
        if (currN && !prevTeclaN) {
            modoCamara = 4;
            piActual = (piActual + 1) % 3;
            printf("Camara: punto de interes %d\n", piActual + 1);
        }
        prevTeclaVirgula = currVirgula;
        prevTeclaPunto = currPunto;
        prevTeclaM = currM;
        prevTeclaN = currN;

        // Cámara aérea: desplazamiento WASD sobre plano XZ
        if (modoCamara == 2) {
            if (keys[GLFW_KEY_W]) hpCamAereaPos.z -= hpCamAereaVel * deltaTime;
            if (keys[GLFW_KEY_S]) hpCamAereaPos.z += hpCamAereaVel * deltaTime;
            if (keys[GLFW_KEY_A]) hpCamAereaPos.x -= hpCamAereaVel * deltaTime;
            if (keys[GLFW_KEY_D]) hpCamAereaPos.x += hpCamAereaVel * deltaTime;
            hpCamAereaPos.y = hpOffsetEscena.y + hpCamAereaAltura;
        }

        // ============================================
        // CICLO DÍA/NOCHE  (T = avanzar manualmente)
        // 0=día 45s, 1=atardecer 15s, 2=noche 45s, 3=amanecer 15s → total 120s (2 min)
        // ============================================
        bool currT = keys[GLFW_KEY_T];
        if (currT && !prevTeclaT) { estadoCiclo = (estadoCiclo + 1) % 4; ciclTimer = 0.f; }
        prevTeclaT = currT;

        ciclTimer += deltaTime;
        switch (estadoCiclo) {
        case 0: if (ciclTimer >= duracionDia) { estadoCiclo = 1; ciclTimer = 0.f; } break;
        case 1: if (ciclTimer >= duracionTrans) { estadoCiclo = 2; ciclTimer = 0.f; } break;
        case 2: if (ciclTimer >= duracionDia) { estadoCiclo = 3; ciclTimer = 0.f; } break;
        case 3: if (ciclTimer >= duracionTrans) { estadoCiclo = 0; ciclTimer = 0.f; } break;
        }
        float factorDia = 1.f;
        switch (estadoCiclo) {
        case 0: factorDia = 1.f; break;
        case 1: factorDia = 1.f - ciclTimer / duracionTrans; break;
        case 2: factorDia = 0.f; break;
        case 3: factorDia = ciclTimer / duracionTrans; break;
        }
        float ambActual = 0.05f + (0.3f - 0.05f) * factorDia;
        float diffActual = 0.1f + (0.8f - 0.1f) * factorDia;
        mainLight = DirectionalLight(1.f, 1.f, 1.f, ambActual, diffActual, 0.f, -0.5f, 1.f);

        // ============================================
        // ANIMACIONES CONTINUAS
        // ============================================

        // Copa: flotación + latido + rotación
        copaTime += deltaTime;
        copaRotY += copaRotSpeed * deltaTime;
        if (copaRotY > 360.f) copaRotY -= 360.f;
        float copaOffsetY = copaFloatAmp * sinf(copaTime * copaFloatSpeed);
        float copaHB = fabsf(sinf(copaTime * copaHeartSpeed));
        float copaEscActual = 1.f + copaHeartAmp * copaHB;

        // Plasmido: flotación + latido + rotación
        plasTime += deltaTime;
        plasRotY += plasRotSpeed * deltaTime;
        if (plasRotY >= 360.f) plasRotY -= 360.f;
        float plasOffsetY = plasFloatAmp * sinf(plasTime * plasFloatSpeed);
        float plasHB = fabsf(sinf(plasTime * plasHeartSpeed));
        float plasEscActual = escPlasmido + plasHeartAmp * plasHB;

        // Taladro del Big Daddy (gira mientras camina)
        if (bigDaddyCaminando) {
            taladroRot += 150.f * deltaTime;
            if (taladroRot > 360.f) taladroRot -= 360.f;
        }

        // Caminata automática del Big Daddy por ruta cerrada
        float pasoSeno = 0.f, angPIzq = 0.f, angPDer = 0.f, angBIzq = 0.f, angBDer = 0.f;
        if ((int)rutaBigDaddy.size() >= 2) {
            glm::vec3 dest = rutaBigDaddy[bigDaddyNodoActual];
            glm::vec3 dir = dest - bigDaddyPosActual; dir.y = 0.f;
            float dist = glm::length(dir);
            if (dist > 0.001f) {
                bigDaddyCaminando = true;
                glm::vec3 dn = glm::normalize(dir);
                float paso = bigDaddyVel * deltaTime;
                if (paso >= dist) {
                    bigDaddyPosActual = dest;
                    bigDaddyNodoActual = (bigDaddyNodoActual + 1) % (int)rutaBigDaddy.size();
                }
                else {
                    bigDaddyPosActual += dn * paso;
                }
                bigDaddyPosActual.y = posBigDaddy.y;
                bigDaddyRotY = -glm::degrees(atan2(dn.x, dn.z)) + 180.f;
                caminataTime += deltaTime;
                pasoSeno = sinf(caminataTime * caminataSpeed);
                angPIzq = pasoSeno * caminataAmp;
                angPDer = -pasoSeno * caminataAmp;
                angBIzq = -pasoSeno * caminataBrazoAmp;
                angBDer = pasoSeno * caminataBrazoAmp;
            }
            else {
                bigDaddyPosActual = dest;
                bigDaddyNodoActual = (bigDaddyNodoActual + 1) % (int)rutaBigDaddy.size();
            }
        }
        else {
            bigDaddyCaminando = false;
            bigDaddyPosActual = posBigDaddy;
            bigDaddyRotY = 0.f;
        }

        // Apertura del medkit al acercarse
        float distMK = glm::length(camera.getCameraPosition() - posMedkit);
        puertaTarget = (distMK <= distActivacionMedkit) ? 45.f : 0.f;
        puertaAngulo += puertaSpeed * deltaTime * ((puertaAngulo < puertaTarget) ? 1.f : -1.f);
        puertaAngulo = glm::clamp(puertaAngulo, 0.f, 45.f);

        // Snitch — ruta automática (tecla 3)
        bool currAuto3 = keys[GLFW_KEY_3];
        if (currAuto3 && !prevTeclaAuto3) { hpSnitchAnimActiva = !hpSnitchAnimActiva; hpSnitchVisible = true; }
        prevTeclaAuto3 = currAuto3;
        if (hpSnitchAnimActiva && hpSnitchVisible)
            UpdatePathFollower(hpPosSnitch, hpRotCarro, hpSnitchWP, hpSnitchRuta, hpSnitchVel, deltaTime, 0.80f);

        // Giratiempo — keyframes (tecla 4)
        bool currGT = keys[GLFW_KEY_4];
        if (currGT && !prevTeclaGT) gtAnimActiva = !gtAnimActiva;
        prevTeclaGT = currGT;
        if (gtAnimActiva && (int)gtKeyframes.size() >= 2) {
            glm::vec3 dest = gtKeyframes[(gtNodoActual + 1) % (int)gtKeyframes.size()];
            glm::vec3 dir = dest - gtPosActual;
            float dist = glm::length(dir);
            if (dist < 0.08f) {
                gtNodoActual = (gtNodoActual + 1) % (int)gtKeyframes.size();
            }
            else {
                glm::vec3 dn = glm::normalize(dir);
                float paso = gtVelocidad * deltaTime;
                if (paso > dist) paso = dist;
                gtPosActual += dn * paso;
                gtRotY += 180.f * deltaTime;
                if (gtRotY >= 360.f) gtRotY -= 360.f;
            }
        }
        else {
            gtPosActual = hpPosGiratiempo;
        }

        // Hermione: movimiento con flechas
        bool hpMoving = false;
        if (keys[GLFW_KEY_UP]) { hpPosZ -= hpVelocidad * deltaTime; hpRotPersonaje = 180.f; hpMoving = true; }
        else if (keys[GLFW_KEY_DOWN]) { hpPosZ += hpVelocidad * deltaTime; hpRotPersonaje = 0.f; hpMoving = true; }
        else if (keys[GLFW_KEY_LEFT]) { hpPosX -= hpVelocidad * deltaTime; hpRotPersonaje = -90.f; hpMoving = true; }
        else if (keys[GLFW_KEY_RIGHT]) { hpPosX += hpVelocidad * deltaTime; hpRotPersonaje = 90.f; hpMoving = true; }
        if (hpMoving) {
            float a = sinf(glfwGetTime() * 5.f) * 30.f;
            hpRotBrazoDer = a; hpRotBrazoIzq = -a; hpRotPiernaDer = -a; hpRotPiernaIzq = a;
        }
        else {
            hpRotBrazoDer = hpRotBrazoIzq = hpRotPiernaDer = hpRotPiernaIzq = 0.f;
        }

        // Pista de carreras (tecla 2 toggle)
        velGral = 0.02f;
        static bool teclaPresionada = false;
        if (keys[GLFW_KEY_2]) {
            if (!teclaPresionada) {
                animPista = !animPista;
                if (!animPista) { pistaIdx = 0; pistaT = 0.f; angleY = 0.f; posCars = glm::vec3(-41.6996f, 0.f, 0.f); }
                teclaPresionada = true;
            }
        }
        else { teclaPresionada = false; }
        if (animPista) recorrerPista(deltaTime / velGral);

        // Animación aplasta-cajas (tecla 1)
        animacionAplastaCajas(deltaTime);

        // Tren: KP_0 arranca / frena
        bool currTren = keys[GLFW_KEY_KP_0];
        if (currTren && !prevTeclaTren) {
            trenAnimActiva = !trenAnimActiva;
            if (trenAnimActiva) {
                alSourceStop(audioMgr.sources["trenMov"]);
                alSourceStop(audioMgr.sources["trenFrenando"]);
                alSourcePlay(audioMgr.sources["trenInicio"]);
                alSourcePlay(audioMgr.sources["trenMov"]);
                printf("\nTREN: arrancando\n");
            }
            else {
                alSourceStop(audioMgr.sources["trenMov"]);
                alSourceStop(audioMgr.sources["trenInicio"]);
                alSourcePlay(audioMgr.sources["trenFrenando"]);
                printf("\nTREN: frenando\n");
            }
        }
        prevTeclaTren = currTren;

        if (trenAnimActiva && (int)rutaTren.size() >= 2) {
            glm::vec3 dest = rutaTren[(trenNodoActual + 1) % (int)rutaTren.size()];
            glm::vec3 dir = dest - trenPos; dir.y = 0.f;
            float dist = glm::length(dir);
            if (dist < 0.5f) {
                trenNodoActual = (trenNodoActual + 1) % (int)rutaTren.size();
            }
            else {
                glm::vec3 dn = glm::normalize(dir);
                float paso = trenVelocidad * deltaTime; if (paso > dist) paso = dist;
                trenPos += dn * paso; trenPos.y = alturaVuelo;
                trenRotY = glm::degrees(atan2(dn.x, dn.z));
            }
            trenRotLlantas += 200.f * deltaTime;
            if (trenRotLlantas > 360.f) trenRotLlantas -= 360.f;
        }

        // Radio: KP_5 enciende / apaga (solo si el jugador está cerca)
        bool currRadio = keys[GLFW_KEY_KP_5];
        if (currRadio && !prevTeclaRadio) {
            glm::vec3 hpWorldPosRadio = hpOffsetEscena + glm::vec3(hpPosX, 1.0f, hpPosZ);
            glm::vec3 camPosCheck = (modoCamara == 1) ? hpWorldPosRadio : camera.getCameraPosition();
            float distR = glm::length(camPosCheck - posRadio);

            if (distR <= distActivacionRadio) {
                radioEncendido = !radioEncendido;
                ALuint src = audioMgr.sources["radio"];
                if (radioEncendido) {
                    alSourcePlay(src);
                    printf("\nRADIO: encendido\n");
                }
                else {
                    alSourceStop(src);
                    printf("\nRADIO: apagado\n");
                }
            }
            else {
                printf("\nRADIO: acercate mas (dist: %.1f)\n", distR);
            }
        }
        prevTeclaRadio = currRadio;

        // Spotlight Hermione: toggle con F
        bool currF = keys[GLFW_KEY_F];
        if (currF && !prevTeclaF) {
            hermionaSpotActiva = !hermionaSpotActiva;
            printf("\nSPOT HERMIONE: %s\n", hermionaSpotActiva ? "ON" : "OFF");
        }
        prevTeclaF = currF;

        alSource3f(audioMgr.sources["radio"], AL_POSITION, posRadio.x, posRadio.y, posRadio.z);

        // ============================================
        // CÁLCULO DE VISTA ACTIVA
        // ============================================
        glm::vec3 hpWorldPos = hpOffsetEscena + glm::vec3(hpPosX, 1.f, hpPosZ);
        float hpYawRad = hpRotPersonaje * toRadians;
        glm::vec3 hpForward = glm::normalize(glm::vec3(sinf(hpYawRad), 0.f, cosf(hpYawRad)));

        glm::vec3 camPosActiva(0.f), camTargetActiva(0.f), camDirActiva(0.f, 0.f, -1.f);
        glm::vec3 upVector(0.f, 1.f, 0.f);

        if (modoCamara == 1) {
            // 3ra persona ligada a Hermione (plano XZ)
            camPosActiva = hpWorldPos - hpForward * hpCamDistTP + glm::vec3(0.f, hpCamAlturaTP, 0.f);
            camTargetActiva = hpWorldPos + hpForward * hpCamLookAhead + glm::vec3(0.f, hpCamLookY, 0.f);
        }
        else if (modoCamara == 2) {
            // Camara aerea recta hacia el piso
            camPosActiva = glm::vec3(
                hpCamAereaPos.x,
                hpOffsetEscena.y + 40.0f,
                hpCamAereaPos.z
            );

            camTargetActiva = glm::vec3(
                hpCamAereaPos.x,
                hpOffsetEscena.y,
                hpCamAereaPos.z
            );

            upVector = glm::vec3(0.0f, 0.0f, -1.0f);
        }

        else if (modoCamara == 3) {
            // Libre (WASD + mouse)
            camPosActiva = camera.getCameraPosition();
            camTargetActiva = camPosActiva + camera.getCameraDirection() * 5.f;
        }
        else if (modoCamara == 4) {
            // Punto de interés fijo capturado
            camPosActiva = puntosInteres[piActual].pos;
            camTargetActiva = camPosActiva + puntosInteres[piActual].dir * 5.f;
        }
        else {
            camPosActiva = puntosInteres[0].pos;
            camTargetActiva = camPosActiva + puntosInteres[0].dir * 5.f;
        }

        camDirActiva = glm::normalize(camTargetActiva - camPosActiva);

        if (modoCamara != 3) {
            camera.setPosition(camPosActiva);
            camera.lookAt(camTargetActiva);
        }
        else {
            camPosActiva = camera.getCameraPosition();
            camTargetActiva = camPosActiva + camera.getCameraDirection() * 5.f;
            camDirActiva = camera.getCameraDirection();
        }

        // Vibración de cámara por proximidad al Big Daddy
        float distCamBD = glm::length(camPosActiva - bigDaddyPosActual);
        float factorCerc = glm::clamp(1.f - distCamBD / vibracionRangoMax, 0.f, 1.f);
        float vibX = sinf(caminataTime * caminataSpeed * 2.f) * vibracionAmp * factorCerc;
        float vibY = fabsf(sinf(caminataTime * caminataSpeed * 2.f)) * vibracionAmp * factorCerc;

        glm::mat4 viewMatrix = glm::lookAt(camPosActiva, camTargetActiva, upVector);
        viewMatrix = glm::translate(viewMatrix, glm::vec3(vibX, vibY, 0.f));

        // ============================================
        // ACTUALIZACIÓN DE LUCES CADA FRAME
        // ============================================
        bool farolaEncendida = (factorDia < 0.5f);
        float ambFarola = farolaEncendida ? 0.10f : 0.f;
        float diffFarola = farolaEncendida ? 2.20f : 0.f;

        // Plasmido: luz roja sigue la posición de flotación
        pointLights[1] = PointLight(1.f, 0.f, 0.f, 0.f, 1.f,
            posPlasmido.x, posPlasmido.y + plasOffsetY, posPlasmido.z, 0.5f, 0.3f, 0.2f);

        // Copa HP: desactivada
        pointLights[2] = PointLight(0.53f, 0.81f, 0.98f, 0.f, 0.f,
            hpPosCopa.x, hpPosCopa.y + copaOffsetY, hpPosCopa.z, 0.3f, 0.2f, 0.1f);

        // Big Daddy: amarillo, solo de noche
        {
            float df = farolaEncendida ? 2.5f : 0.f;
            pointLights[3] = PointLight(1.f, 0.85f, 0.2f, 0.f, df,
                bigDaddyPosActual.x, bigDaddyPosActual.y + 5.f, bigDaddyPosActual.z, 0.3f, 0.1f, 0.05f);
        }

        // Hermione: blanco, siempre activo
        {
            glm::vec3 wp = hpOffsetEscena + glm::vec3(hpPosX, 3.f, hpPosZ);
            pointLights[4] = PointLight(1.f, 1.f, 1.f, 0.f, 1.f, wp.x, wp.y, wp.z, 0.5f, 0.2f, 0.1f);
        }

        // Radio: naranja, solo de noche
        {
            float df = farolaEncendida ? 3.f : 0.f;
            pointLights[0] = PointLight(1.f, 0.45f, 0.f, 0.f, df,
                posRadio.x, posRadio.y + 1.f, posRadio.z, 0.5f, 0.2f, 0.1f);
        }

        // Farola puntual: selecciona la más cercana al jugador entre las 5 farolas
        {
            float df = farolaEncendida ? 4.f : 0.f;
            glm::vec3 fps[5] = {
                {3.f,14.76f,117.09f}, {35.15f,14.76f,46.53f},
                {-32.83f,14.76f,36.22f}, {-51.33f,14.76f,-62.83f},
                {65.49f,14.76f,-63.91f}
            };
            int idx = 0; float best = glm::length(camPosActiva - fps[0]);
            for (int f = 1; f < 5; f++) { float d = glm::length(camPosActiva - fps[f]); if (d < best) { best = d; idx = f; } }
            pointLights[5] = PointLight(1.f, 0.93f, 0.72f, 0.f, df,
                fps[idx].x, fps[idx].y, fps[idx].z, 0.3f, 0.03f, 0.001f);
        }

        // SpotLights de farola: calculadas desde la posición de la farola 1
        glm::mat4 baseFarola = glm::mat4(1.f);
        baseFarola = glm::translate(baseFarola, posFarola);
        baseFarola = glm::rotate(baseFarola, rotFarola.x * toRadians, glm::vec3(1, 0, 0));
        baseFarola = glm::rotate(baseFarola, rotFarola.y * toRadians, glm::vec3(0, 1, 0));
        baseFarola = glm::rotate(baseFarola, rotFarola.z * toRadians, glm::vec3(0, 0, 1));
        glm::vec3 fDerW = glm::vec3(baseFarola * glm::vec4(luzLocalFocoDer, 1.f));
        glm::vec3 fIzqW = glm::vec3(baseFarola * glm::vec4(luzLocalFocoIzq, 1.f));
        glm::vec3 dDerW = glm::normalize(glm::mat3(baseFarola) * dirLocalFocoDer);
        glm::vec3 dIzqW = glm::normalize(glm::mat3(baseFarola) * dirLocalFocoIzq);
        spotLights[2] = SpotLight(1.f, 0.93f, 0.72f, ambFarola, diffFarola, fDerW.x, fDerW.y, fDerW.z, dDerW.x, dDerW.y, dDerW.z, 1.f, 0.04f, 0.01f, 40.f);
        spotLights[3] = SpotLight(1.f, 0.93f, 0.72f, ambFarola, diffFarola, fIzqW.x, fIzqW.y, fIzqW.z, dIzqW.x, dIzqW.y, dIzqW.z, 1.f, 0.04f, 0.01f, 40.f);

        // Linterna de Hermione: toggle F, apunta hacia donde mira el avatar
        {
            glm::vec3 wp = hpOffsetEscena + glm::vec3(hpPosX, 2.f, hpPosZ);
            float yaw = hpRotPersonaje * toRadians;
            glm::vec3 fwd = glm::normalize(glm::vec3(sinf(yaw), -0.3f, cosf(yaw)));
            float df = hermionaSpotActiva ? 3.f : 0.f;
            spotLights[4] = SpotLight(1.f, 1.f, 0.9f, 0.f, df, wp.x, wp.y, wp.z, fwd.x, fwd.y, fwd.z, 1.f, 0.f, 0.f, 20.f);
        }

        // Foco frontal del tren: solo de noche, apunta hacia donde avanza
        {
            float df = farolaEncendida ? 4.f : 0.f;
            glm::vec3 fwd = glm::vec3(sinf(trenRotY * toRadians), 0.f, cosf(trenRotY * toRadians));
            spotLights[5] = SpotLight(1.f, 0.95f, 0.6f, 0.f, df, trenPos.x, trenPos.y, trenPos.z, fwd.x, fwd.y, fwd.z, 1.f, 0.f, 0.f, 15.f);
        }

        // Linterna de cámara
        glm::vec3 llPos = camPosActiva; llPos.y -= 0.3f;
        spotLights[0].SetFlash(llPos, camDirActiva);

        // Audio listener sigue la cámara activa
        audioMgr.SetListenerPos(camPosActiva, camDirActiva, upVector);
        audioMgr.PlayAudio("ambiente", glm::vec3(0.f));
        audioMgr.PlayAudio("bigdaddy", bigDaddyPosActual);

        // Engranes del reloj: órbita continua
        angE1 -= velE1 * deltaTime; if (angE1 <= -360.f) angE1 += 360.f;
        angE2 += velE2 * deltaTime; if (angE2 >= 360.f) angE2 -= 360.f;
        angE3 -= velE3 * deltaTime; if (angE3 <= -360.f) angE3 += 360.f;
        angE4 += velE4 * deltaTime; if (angE4 >= 360.f) angE4 -= 360.f;
        angGR -= velGR * deltaTime; if (angGR <= -360.f) angGR += 360.f;
        angPL -= velPL * deltaTime; if (angPL <= -360.f) angPL += 360.f;

        // ============================================
        // RENDER
        // ============================================
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (factorDia >= 0.5f)
            skyboxDia.DrawSkybox(camera.calculateViewMatrix(), projection);
        else
            skyboxNoche.DrawSkybox(camera.calculateViewMatrix(), projection);

        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformEyePosition = shaderList[0].GetEyePositionLocation();
        uniformColor = shaderList[0].getColorLocation();
        uniformTextureOffset = shaderList[0].getOffsetLocation();
        uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
        uniformShininess = shaderList[0].GetShininessLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniform3f(uniformEyePosition, camPosActiva.x, camPosActiva.y, camPosActiva.z);

        glm::vec3 llPosR = camPosActiva; llPosR.y -= 0.3f;
        spotLights[0].SetFlash(llPosR, camDirActiva);

        shaderList[0].SetDirectionalLight(&mainLight);
        shaderList[0].SetPointLights(pointLights, pointLightCount);
        shaderList[0].SetSpotLights(spotLights, spotLightCount);

        glm::vec3 colorBlanco = glm::vec3(1.f);
        glm::vec2 offsetCero = glm::vec2(0.f);
        glUniform3fv(uniformColor, 1, glm::value_ptr(colorBlanco));
        glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(offsetCero));

        glm::mat4 model = glm::mat4(1.f);

        // --- PISO ---
        model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -2.f, 0.f));
        model = glm::scale(model, glm::vec3(30.f, 1.f, 30.f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        pisoTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[0]->RenderMesh();

        // --- MURALLAS ---
        model = ApplyTRS(glm::mat4(1.f), posMurallaL_1, rotMurallaL_1, glm::vec3(escMurallaL) * scaleMurallaL);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        Pared_M.RenderModel();

        model = ApplyTRS(glm::mat4(1.f), posMurallaL_2, rotMurallaL_2, glm::vec3(escMurallaL) * scaleMurallaL);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
        Pared_M.RenderModel();

        // --- FAROLAS (5 instancias, escala doble) ---
        {
            glm::vec3 posF[5] = {
                {3.f,-2.f,117.09f}, {35.15f,-2.f,46.53f},
                {-32.83f,-2.f,36.22f}, {-51.33f,-2.f,-62.83f},
                {65.49f,-2.f,-63.91f}
            };
            for (int f = 0; f < 5; f++) {
                glm::mat4 base = glm::translate(glm::mat4(1.f), posF[f]);
                model = ApplyTRS(base, offsetPaloFarola, rotPaloFarola, glm::vec3(escFarola * 2.f) * scalePaloFarola);
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
                PaloFarola_M.RenderModel();
                model = ApplyTRS(base, offsetFocoDerFarola * 2.f, rotFocoDerFarola, glm::vec3(escFarola * 2.f) * scaleFocoDerFarola);
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
                FocoDer_M.RenderModel();
                model = ApplyTRS(base, offsetFocoIzqFarola * 2.f, rotFocoIzqFarola, glm::vec3(escFarola * 2.f) * scaleFocoIzqFarola);
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
                FocoIzq_M.RenderModel();
            }
        }

        // --- BANCAS (4 instancias, orientadas según captura) ---
        {
            struct BancaInst { glm::vec3 pos; float rotY; };
            BancaInst bancas[4] = {
                { {105.79f,-2.f,-68.05f},  glm::degrees(atan2f(-0.994f, 0.010f)) - 90.f },
                { {-44.76f,-2.f,-119.20f}, glm::degrees(atan2f(0.010f, 1.000f)) - 90.f },
                { {-106.85f,-2.f,52.99f},  glm::degrees(atan2f(0.990f,-0.057f)) - 90.f },
                { {43.78f,-2.f,112.91f},   glm::degrees(atan2f(-0.021f,-0.994f)) - 90.f }
            };
            for (int b = 0; b < 4; b++) {
                model = glm::translate(glm::mat4(1.f), bancas[b].pos);
                model = glm::rotate(model, bancas[b].rotY * toRadians, glm::vec3(0, 1, 0));
                model = glm::scale(model, glm::vec3(1.f));
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
                Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
                Banca_M.RenderModel();
            }
        }

        // --- BLOQUES DE PASTO (8 instancias en anillo) ---
        for (int k = 0; k < 12; k++) {
            model = ApplyTRS(glm::mat4(1.f), pastoPos[k], pastoRot[k], glm::vec3(escBloquePasto) * scaleBloquePasto);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            BloquePasto_M.RenderModel();
        }

        // --- RELOJ DE PLANETAS (jerarquía con órbita) ---
        glm::mat4 baseReloj = glm::translate(glm::mat4(1.f), posReloj);
        baseReloj = glm::rotate(baseReloj, rotReloj.x * toRadians, glm::vec3(1, 0, 0));
        baseReloj = glm::rotate(baseReloj, rotReloj.y * toRadians, glm::vec3(0, 1, 0));
        baseReloj = glm::rotate(baseReloj, rotReloj.z * toRadians, glm::vec3(0, 0, 1));

        auto renderRelojPieza = [&](glm::vec3 off, glm::vec3 rot, glm::vec3 sc, Model& mdl, float ang = 0.f) {
            glm::mat4 m = baseReloj;
            if (ang != 0.f) m = glm::rotate(m, ang * toRadians, glm::vec3(0, 1, 0));
            m = glm::translate(m, off);
            m = glm::rotate(m, rot.x * toRadians, glm::vec3(1, 0, 0));
            m = glm::rotate(m, rot.y * toRadians, glm::vec3(0, 1, 0));
            m = glm::rotate(m, rot.z * toRadians, glm::vec3(0, 0, 1));
            m = glm::scale(m, sc);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            mdl.RenderModel();
        };
        renderRelojPieza(offsetBasePlanetas, rotBasePlanetas, glm::vec3(escReloj) * scaleBasePlanetas, BasePlanetas_M);
        renderRelojPieza(offsetRelojBase, rotRelojBase, glm::vec3(escReloj) * scaleRelojBase, RelojBase_M);
        renderRelojPieza(offsetRelojEngrane1, rotRelojEngrane1, glm::vec3(escReloj) * scaleRelojEngrane1, RelojEngrane1_M, angE1);
        renderRelojPieza(offsetRelojEngrane2, rotRelojEngrane2, glm::vec3(escReloj) * scaleRelojEngrane2, RelojEngrane2_M, angE2);
        renderRelojPieza(offsetRelojEngrane3, rotRelojEngrane3, glm::vec3(escReloj) * scaleRelojEngrane3, RelojEngrane3_M, angE3);
        renderRelojPieza(offsetRelojEngrane4, rotRelojEngrane4, glm::vec3(escReloj) * scaleRelojEngrane4, RelojEngrane4_M, angE4);
        renderRelojPieza(offsetRelojGalRedE, rotRelojGalRedE, glm::vec3(escReloj) * scaleRelojGalRedE, RelojGalRedE_M, angGR);
        renderRelojPieza(offsetRelojPlanetas, rotRelojPlanetas, glm::vec3(escReloj) * scaleRelojPlanetas, RelojPlanetas_M, angPL);

        // --- BIG DADDY (jerarquía completa animada) ---
        glm::mat4 baseBD = glm::translate(glm::mat4(1.f), bigDaddyPosActual);
        baseBD = glm::rotate(baseBD, bigDaddyRotY * toRadians, glm::vec3(0, 1, 0));

        auto renderBD = [&](glm::vec3 off, glm::vec3 rot, float esc, Model& mdl) {
            glm::mat4 m = glm::translate(baseBD, off);
            m = glm::rotate(m, rot.x * toRadians, glm::vec3(1, 0, 0));
            m = glm::rotate(m, rot.y * toRadians, glm::vec3(0, 1, 0));
            m = glm::rotate(m, rot.z * toRadians, glm::vec3(0, 0, 1));
            m = glm::scale(m, glm::vec3(esc));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            mdl.RenderModel();
        };
        renderBD(offsetCuerpo, rotCuerpo, escBigDaddy, BigDaddyCuerpo_M);
        renderBD(offsetBrazoIzq, { rotBrazoIzq.x + angBIzq,  rotBrazoIzq.y,  rotBrazoIzq.z }, escBigDaddy, BigDaddyBrazoIzq_M);
        renderBD(offsetBrazoDer, { rotBrazoDer.x + angBDer,   rotBrazoDer.y,  rotBrazoDer.z }, escBigDaddy, BigDaddyBrazoDer_M);
        renderBD(offsetPiernaIzq, { rotPiernaIzq.x + angPIzq,  rotPiernaIzq.y, rotPiernaIzq.z }, escBigDaddy, BigDaddyPiernaIzq_M);
        renderBD(offsetPiernaDer, { rotPiernaDer.x + angPDer,  rotPiernaDer.y, rotPiernaDer.z }, escBigDaddy, BigDaddyPiernaDer_M);
        // Taladro: espejado en X para que coincida el mesh
        {
            glm::mat4 m = glm::translate(baseBD, offsetTaladro * escBigDaddy);
            m = glm::rotate(m, -angBDer * toRadians, glm::vec3(1, 0, 0));
            m = glm::rotate(m, rotTaladro.z * toRadians, glm::vec3(0, 0, 1));
            m = glm::rotate(m, taladroRot * toRadians, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(-escBigDaddy, escBigDaddy, escBigDaddy));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            glCullFace(GL_FRONT);
            BigDaddyTaladro_M.RenderModel();
            glCullFace(GL_BACK);
        }

        // --- MEDKIT (abre tapas al acercarse) ---
        glm::mat4 baseMK = glm::translate(glm::mat4(1.f), posMedkit);
        auto renderMK = [&](glm::vec3 off, float angX, Model& mdl) {
            glm::mat4 m = glm::translate(baseMK, off);
            m = glm::rotate(m, 180.f * toRadians, glm::vec3(0, 1, 0));
            m = glm::rotate(m, angX * toRadians, glm::vec3(1, 0, 0));
            m = glm::scale(m, glm::vec3(-escMedkit, escMedkit, escMedkit));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            glCullFace(GL_FRONT); mdl.RenderModel(); glCullFace(GL_BACK);
        };
        {
            glm::mat4 m = glm::scale(baseMK, glm::vec3(-escMedkit, escMedkit, escMedkit));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            glCullFace(GL_FRONT); BolsaMid_M.RenderModel(); glCullFace(GL_BACK);
        }
        renderMK(offsetBolsaDer, puertaAngulo, BolsaDer_M);
        renderMK(offsetBolsaIzq, -puertaAngulo, BolsaIzq_M);

        // --- PLASMIDO (flotación + latido + rotación) ---
        {
            glm::mat4 m = glm::translate(glm::mat4(1.f), posPlasmido + glm::vec3(0.f, plasOffsetY, 0.f));
            m = glm::rotate(m, plasRotY * toRadians, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(plasEscActual));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Plasmido_M.RenderModel();
        }

        // --- RADIO ---
        {
            glm::mat4 m = glm::translate(glm::mat4(1.f), posRadio);
            m = glm::rotate(m, glm::degrees(atan2f(-0.998f, 0.058f)) * toRadians, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(1.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Radio_M.RenderModel();
        }

        // --- HERMIONE (jerarquía completa animada) ---
        {
            glm::mat4 base = glm::translate(glm::mat4(1.f), hpOffsetEscena + glm::vec3(hpPosX, 1.f, hpPosZ));
            base = glm::rotate(base, hpRotPersonaje * toRadians, glm::vec3(0, 1, 0));
            base = glm::scale(base, glm::vec3(1.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Hermione_HP_M.RenderModel();

            auto miembro = [&](glm::vec3 off, float rot, Model& mdl) {
                glm::mat4 m = glm::translate(base, off);
                m = glm::rotate(m, rot * toRadians, glm::vec3(1, 0, 0));
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
                Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
                mdl.RenderModel();
            };
            miembro({ 0.5f,0.8f,0.f }, hpRotBrazoDer, BrazoDer_HP_M);
            miembro({ -0.3f,0.8f,0.f }, hpRotBrazoIzq, BrazoIzq_HP_M);
            miembro({ -0.1f,-0.7f,0.f }, hpRotPiernaDer, PiernaDer_HP_M);
            miembro({ 0.1f,-0.7f,0.f }, hpRotPiernaIzq, PiernaIzq_HP_M);
        }

        // --- GIRATIEMPO (keyframes, tecla 4) ---
        {
            glm::mat4 m = glm::translate(glm::mat4(1.f), gtPosActual);
            m = glm::rotate(m, gtRotY * toRadians, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(0.5f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Giratiempo_HP_M.RenderModel();
        }

        // --- LIBRO DE HECHIZOS ---
        {
            glm::mat4 m = glm::translate(glm::mat4(1.f), hpPosLibro);
            m = glm::scale(m, glm::vec3(2.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            LibroHechizos_HP_M.RenderModel();
        }

        // --- SNITCH (ruta automática, tecla 3) ---
        if (hpSnitchVisible) {
            glm::mat4 m = glm::translate(glm::mat4(1.f), hpPosSnitch);
            m = glm::scale(m, glm::vec3(2.4f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Snitch_HP_M.RenderModel();
        }

        // --- COPA (flotación + latido + rotación) ---
        {
            glm::mat4 m = glm::translate(glm::mat4(1.f), hpPosCopa + glm::vec3(0.f, copaOffsetY, 0.f));
            m = glm::rotate(m, copaRotY * toRadians, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(copaEscActual));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Copa_HP_M.RenderModel();
        }

        // --- TREN EXPRESO DE HOGWARTS (keyframes, KP_0) ---
        {
            glm::mat4 mt = glm::translate(glm::mat4(1.f), trenPos);
            mt = glm::rotate(mt, trenRotY * toRadians, glm::vec3(0, 1, 0));
            mt = glm::scale(mt, glm::vec3(2.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mt));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            ExpresoHowarts_M.RenderModel();
            glm::mat4 ml = glm::translate(mt, glm::vec3(0.f, 0.f, 0.5f));
            ml = glm::rotate(ml, trenRotLlantas * toRadians, glm::vec3(1, 0, 0));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(ml));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            LlantaSteamCar3_M.RenderModel();
        }

        // --- CRASH BANDICOOT (jerarquía, tecla 1 = tornado) ---
        {
            glm::mat4 mc = glm::translate(glm::mat4(1.f), glm::vec3(-63.f, 2.f, 80.f));
            mc = glm::rotate(mc, rotacionTornado * toRadians, glm::vec3(0, 1, 0));
            mc = glm::rotate(mc, 180.f * toRadians, glm::vec3(0, 1, 0));
            mc = glm::scale(mc, glm::vec3(3.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mc));
            Crash_Cuerpo_M.RenderModel();

            auto miembroCrash = [&](glm::vec3 off, float rot, Model& mdl) {
                glm::mat4 m = glm::translate(mc, off);
                m = glm::rotate(m, rot * toRadians, glm::vec3(1, 0, 0));
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
                mdl.RenderModel();
            };
            miembroCrash({ -0.252f,0.0049f,0.0255f }, 0.f, Crash_BrazoDer_M);
            miembroCrash({ 0.235f,-0.005f,0.019f }, 0.f, Crash_BrazoIzq_M);
            miembroCrash({ 0.f,-0.589f,0.008f }, 0.f, Crash_PiernaDer_M);
            miembroCrash({ -0.005f,-0.587f,0.008f }, 0.f, Crash_PiernaIzq_M);
        }

        // --- BOLA DE ROCA ---
        {
            glm::mat4 m = glm::translate(glm::mat4(1.f), posBolaRoca);
            m = glm::rotate(m, rotacionRocaZ * toRadians, glm::vec3(0, 0, 1));
            m = glm::scale(m, glm::vec3(8.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            BolaRoca_M.RenderModel();
        }

        // --- CAJAS APLASTABLES ---
        auto renderCaja = [&](glm::vec3 pos, float escY, Model& mdl) {
            glm::mat4 m = glm::translate(glm::mat4(1.f), pos);
            m = glm::scale(m, glm::vec3(5.f, escY, 5.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            mdl.RenderModel();
        };
        renderCaja(posCajaAku, escAku, CajaAku_M);
        renderCaja(posCajaCaraCrash, escCara, CajaCaraCrash_M);
        renderCaja(posCajaCheckPoint, escCheck, CajaCheckPoint_M);
        renderCaja(posCajaFlechaArriba, escFlecha, CajaFlechaArriba_M);
        renderCaja(posCajaMetalExcl, escMetal, CajaMetalExclamation_M);
        renderCaja(posCajaNitro, escNitro, CajaNitro_M);
        renderCaja(posCajaNitroExcl, escNitroExcl, CajaNitroExclamation_M);
        renderCaja(posCajaNormal, escNormal, CajaNormalCrash_M);
        renderCaja(posCajaQuestion, escQuest, CajaQuestion_M);
        renderCaja(posCajaTNT, escTNT, CajaTNT_M);
        {
            glm::mat4 m = glm::translate(glm::mat4(1.f), posTotem1Crash);
            m = glm::scale(m, glm::vec3(5.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Totem1Crash_M.RenderModel();
        }

        // --- PEDESTALES DECORATIVOS (Crash / Coco / Cortex en bases giratorias) ---
        float tiempo = glfwGetTime();
        auto pedestal = [&](glm::vec3 pos, glm::vec3 posKart, glm::vec3 scKart, float velGiro, Model& kart) {
            glm::mat4 mb = glm::translate(glm::mat4(1.f), pos);
            mb = glm::scale(mb, glm::vec3(5.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mb));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            BaseCilindro_M.RenderModel();
            glm::mat4 mk = glm::translate(glm::mat4(1.f), posKart);
            mk = glm::scale(mk, scKart);
            mk = glm::rotate(mk, glm::radians(tiempo * velGiro), glm::vec3(0, 1, 0));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mk));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            kart.RenderModel();
        };
        pedestal({ -160.f,-2.f,0.f }, { -160.f,4.f,0.f }, glm::vec3(4.f), 50.f, Crash_GoKart_M);
        pedestal({ -160.f,-2.f,20.f }, { -160.f,4.f,20.f }, glm::vec3(2.f), -50.f, CocoOnGoKart_M);
        pedestal({ -160.f,-2.f,-20.f }, { -160.f,4.f,-20.f }, glm::vec3(2.f), -50.f, CortexOnGoKart_M);

        // --- ÁRBOLES WUMPA ---
        for (float ang : {180.f, 90.f}) {
            glm::mat4 m = glm::translate(glm::mat4(1.f), glm::vec3(165.f, -1.f, 0.f));
            m = glm::rotate(m, ang * toRadians, glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(5.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            ArbolWumpa_M.RenderModel();
        }

        // --- TOTEM 2 + CRISTAL GIRATORIO ---
        {
            glm::mat4 m = glm::translate(glm::mat4(1.f), glm::vec3(160.f, -2.f, 0.f));
            m = glm::scale(m, glm::vec3(3.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Totem2Crash_M.RenderModel();
        }
        {
            float y = 6.f + glm::sin(tiempo * 2.f) * 0.5f;
            glm::mat4 m = glm::translate(glm::mat4(1.f), glm::vec3(160.f, y, 0.f));
            m = glm::rotate(m, glm::radians(tiempo * 50.f), glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(0.5f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
            CrystalCrash_M.RenderModel();
        }

        // --- FRUTAS WUMPA (borde cuadrado, animación senoidal individual) ---
        {
            float lado = 360.f;
            int   nFrutas = 50;
            float pasoF = lado / nFrutas;   // nombre distinto para no colisionar con pasoCaja
            float tg = glfwGetTime();
            float bx = -180.f, bz = -180.f;
            for (int fi = 0; fi < 4; fi++) for (int fj = 0; fj < nFrutas; fj++) {
                float fx = 0.f, fz = 0.f;
                if (fi == 0) { fx = fj * pasoF; fz = 0.f; }
                if (fi == 1) { fx = lado; fz = fj * pasoF; }
                if (fi == 2) { fx = (nFrutas - fj) * pasoF; fz = lado; }
                if (fi == 3) { fx = 0.f; fz = (nFrutas - fj) * pasoF; }
                float desfase = (fi * nFrutas + fj) * 0.5f;
                float brinco = glm::sin(tg * 3.f + desfase) * 0.8f;
                glm::mat4 m = glm::translate(glm::mat4(1.f), glm::vec3(bx + fx, 1.f + brinco, bz + fz));
                m = glm::rotate(m, glm::radians(tg * 100.f), glm::vec3(0, 1, 0));
                m = glm::scale(m, glm::vec3(1.f));
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
                Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
                FrutaWumpa_M.RenderModel();
            }
        }

        // --- MÁSCARAS AKU/UKA CON HUMO (animación continua) ---
        rotHumoAku += 0.5f * (deltaTime * 50.f);
        if (creceHumoAltoAku >= 20.f) { creceHumoAltoAku = 0.f; creceHumoAnchoAku = 0.f; }
        creceHumoAltoAku += 0.1f * (deltaTime * 50.f);
        if (creceHumoAnchoAku <= 7.f) creceHumoAnchoAku += 0.1f * (deltaTime * 50.f);

        auto renderMascara = [&](glm::vec3 pos, float rotY, glm::vec3 sc, Model& mdl, float offsetHumoY) {
            glm::mat4 base = glm::translate(glm::mat4(1.f), pos);
            base = glm::rotate(base, rotY * toRadians, glm::vec3(0, 1, 0));
            glm::mat4 m = glm::scale(base, sc);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
            mdl.RenderModel();
            glm::mat4 mh = glm::translate(base, glm::vec3(0.f, offsetHumoY, 0.f));
            mh = glm::rotate(mh, 90.f * toRadians, glm::vec3(1, 0, 0));
            mh = glm::scale(mh, glm::vec3(creceHumoAnchoAku, creceHumoAltoAku, creceHumoAnchoAku));
            mh = glm::rotate(mh, rotHumoAku * toRadians, glm::vec3(0, 1, 0));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mh));
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            humoTexture.UseTexture();
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            meshList[1]->RenderMesh();
            glDisable(GL_BLEND);
        };
        renderMascara({ -100.f,2.f,100.f }, 135.f, glm::vec3(50.f), MascaraAku_M, -2.0f);
        renderMascara({ 100.f,2.f,100.f }, -135.f, glm::vec3(50.f), MascaraAku_M, -2.0f);
        renderMascara({ 100.f,2.f,-100.f }, -45.f, glm::vec3(2.5f), MascaraUka_M, -1.5f);
        renderMascara({ -100.f,2.f,-100.f }, 45.f, glm::vec3(2.5f), MascaraUka_M, -1.5f);

        // --- PISTA DE CARRERAS Y VEHÍCULOS ---
        glm::mat4 modelPista = glm::translate(glm::mat4(1.f), posPista);
        modelPista = glm::scale(modelPista, glm::vec3(3.4f, 4.f, 4.f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPista));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        Pista_M.RenderModel();

        // Carro 1: Crash & Grunt (SteamCar1 + ruedas + humo)
        {
            glm::mat4 c1 = modelPista;
            c1 = glm::translate(c1, { posCars.x + offsetCentroPistaCar1, posCars.y + offsetAlturaPistaCar1, -posCars.z });
            c1 = glm::rotate(c1, glm::radians(-angleY), glm::vec3(0, 1, 0));
            c1 = glm::rotate(c1, -180.f * toRadians, glm::vec3(0, 1, 0));
            c1 = glm::rotate(c1, glm::radians(angleX), glm::vec3(1, 0, 0));
            c1 = glm::scale(c1, glm::vec3(1.f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(c1));
            SteamCar1_M.RenderModel();
            rotHumoCar1 += 5.f;
            if (creceHumoCar1 >= 5.f) creceHumoCar1 = 2.f;
            creceHumoCar1 += 0.5f * (deltaTime * 50.f);
            glm::mat4 mh = c1;
            mh = glm::translate(mh, { 0.f,0.3f,0.f });
            mh = glm::rotate(mh, -90.f * toRadians, glm::vec3(1, 0, 0));
            mh = glm::scale(mh, { 2.f,creceHumoCar1,1.f });
            mh = glm::rotate(mh, rotHumoCar1 * toRadians, glm::vec3(0, 1, 0));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mh));
            glm::vec3 cb = glm::vec3(1.f); glUniform3fv(uniformColor, 1, glm::value_ptr(cb));
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            humoTexture.UseTexture(); Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            meshList[1]->RenderMesh(); glDisable(GL_BLEND);
            rotLlantaCar1 += ((deltaTime * 50.f) / velGral) / 0.03f;
            glm::vec3 posRL[4] = { {0.67f,0.38f,1.22f},{-0.67f,0.38f,1.22f},{0.67f,0.38f,-1.06f},{-0.67f,0.38f,-1.06f} };
            for (auto& p : posRL) { glm::mat4 r = glm::translate(c1, p); r = glm::rotate(r, rotLlantaCar1 * toRadians, glm::vec3(1, 0, 0)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(r)); LlantaFD_Crash_M.RenderModel(); }
        }

        // Carro 2: Master Chief & Coco (SteamCar2 + ruedas + humo)
        {
            glm::mat4 c2 = modelPista;
            c2 = glm::translate(c2, { posCars.x + offsetCentroPistaCar2,posCars.y + offsetAlturaPistaCar2,-posCars.z });
            c2 = glm::rotate(c2, glm::radians(-angleY), glm::vec3(0, 1, 0));
            c2 = glm::rotate(c2, glm::radians(-angleX), glm::vec3(1, 0, 0));
            c2 = glm::rotate(c2, -90.f * toRadians, glm::vec3(0, 1, 0));
            c2 = glm::scale(c2, glm::vec3(0.5f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(c2));
            SteamCar2_M.RenderModel();
            rotHumoCar2 += 0.5f; if (creceHumoCar2 >= 5.f)creceHumoCar2 = 3.f;
            creceHumoCar2 += 0.1f * (deltaTime * 50.f);
            glm::mat4 mh = c2;
            mh = glm::translate(mh, { -0.6f,4.f,-0.05f });
            mh = glm::rotate(mh, -90.f * toRadians, glm::vec3(0, 1, 0));
            mh = glm::scale(mh, { 1.f,creceHumoCar2,1.f });
            mh = glm::rotate(mh, rotHumoCar2 * toRadians, glm::vec3(0, 1, 0));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mh));
            glm::vec3 cb = glm::vec3(1.f); glUniform3fv(uniformColor, 1, glm::value_ptr(cb));
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            humoTexture.UseTexture(); Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            meshList[1]->RenderMesh(); glDisable(GL_BLEND);
            rotLlantaCar2 += ((deltaTime * 50.f) / velGral) / 0.03f;
            // Llanta delanteras
            glm::mat4 r; r = glm::translate(c2, { -0.55f,1.35f,0.5f }); r = glm::rotate(r, rotLlantaCar2 * toRadians, glm::vec3(0, 0, 1)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(r)); RuedaSteamCar2_M.RenderModel();
            r = glm::translate(c2, { -0.55f,1.35f,-2.f }); r = glm::rotate(r, 180.f * toRadians, glm::vec3(0, 1, 0)); r = glm::rotate(r, -rotLlantaCar2 * toRadians, glm::vec3(0, 0, 1)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(r)); RuedaSteamCar2_M.RenderModel();
            // Llantas traseras (escala 1.7)
            r = glm::scale(c2, glm::vec3(1.7f)); r = glm::translate(r, { 1.87f,1.05f,0.45f }); r = glm::rotate(r, rotLlantaCar2 * toRadians, glm::vec3(0, 0, 1)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(r)); RuedaSteamCar2_M.RenderModel();
            r = glm::scale(c2, glm::vec3(1.7f)); r = glm::rotate(r, 180.f * toRadians, glm::vec3(0, 1, 0)); r = glm::translate(r, { -1.87f,1.05f,1.3f }); r = glm::rotate(r, -rotLlantaCar2 * toRadians, glm::vec3(0, 0, 1)); glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(r)); RuedaSteamCar2_M.RenderModel();
        }

        // Carro 3: Harry Potter en moto (SteamCar3 + ruedas + humo)
        {
            glm::mat4 c3 = modelPista;
            c3 = glm::translate(c3, { 0.f,-2.f,0.f });
            c3 = glm::translate(c3, { posCars.x + offsetCentroPistaCar3,posCars.y + offsetAlturaPistaCar3,-posCars.z });
            c3 = glm::rotate(c3, glm::radians(-angleY), glm::vec3(0, 1, 0));
            c3 = glm::rotate(c3, 180.f * toRadians, glm::vec3(0, 1, 0));
            c3 = glm::rotate(c3, glm::radians(angleX), glm::vec3(1, 0, 0));
            c3 = glm::scale(c3, glm::vec3(1.5f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(c3));
            SteamCar3_M.RenderModel();
            rotHumoCar3 += 4.f; if (creceHumoCar3 >= 5.f)creceHumoCar3 = 3.f;
            creceHumoCar3 += 0.5f * (deltaTime * 50.f);
            auto humoMoto = [&](glm::vec3 off, float sy) {
                glm::mat4 mh = glm::translate(c3, off);
                mh = glm::rotate(mh, -90.f * toRadians, glm::vec3(1, 0, 0));
                mh = glm::scale(mh, { 0.7f,sy,0.7f });
                mh = glm::rotate(mh, rotHumoCar3 * toRadians, glm::vec3(0, 1, 0));
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mh));
                glm::vec3 cb = glm::vec3(1.f); glUniform3fv(uniformColor, 1, glm::value_ptr(cb));
                glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                humoTexture.UseTexture(); Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
                meshList[1]->RenderMesh(); glDisable(GL_BLEND);
            };
            humoMoto({ 0.f,0.7f,0.5f }, creceHumoCar3);
            humoMoto({ 0.f,0.3f,0.8f }, creceHumoCar3 - 3.f);
            rotLlantaCar3 += ((deltaTime * 50.f) / velGral) / 0.03f;
            struct RuedaMoto { glm::vec3 off; float esc; };
            RuedaMoto ruedas[4] = { {{0.f,0.25f,-0.45f},1.f},{{0.f,0.25f,-0.97f},1.f},{{0.23f,0.255f,0.89f},1.15f},{{-0.23f,0.255f,0.89f},1.15f} };
            for (auto& r : ruedas) {
                glm::mat4 rm = glm::scale(c3, glm::vec3(r.esc));
                rm = glm::translate(rm, r.off);
                rm = glm::rotate(rm, rotLlantaCar3 * toRadians, glm::vec3(1, 0, 0));
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(rm));
                LlantaSteamCar3_M.RenderModel();
            }
        }

        // ============================================
        // FIN DEL FRAME
        // ============================================
        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    audioMgr.Cleanup();
    return 0;
}