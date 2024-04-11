#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif
#include "Ponto.h"
#include "Personagens.h"
#include "Temporizador.h"
#include "ListaDeCoresRGB.h"

Temporizador T;
double AccumDeltaT = 0;
Temporizador T2;

Personagens vetorDePersonagens[10];
Ponto vetorPontos[10];
int vidasDisparador = 3;

// Limites logicos da area de desenho
Ponto Min, Max;

bool desenha = false;

Poligono Mapa, Disparador, NaveInimiga, Bala;
int nInstanciasPersonagens = 0;
int nInstanciasBalas = 0;
int nPontos = 0;

float angulo = 0.0;

void CriaPersonagens(int numeroDePersonagens);
void CriaBalasNavesInimigas();

void CarregaModelos()
{
    Disparador.LePoligono("Disparador.txt");
    NaveInimiga.LePoligono("NaveInimiga.txt");
    Bala.LePoligono("Bala.txt");
}

void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    float d = 150;
    Min = Ponto(-d, -d);
    Max = Ponto(d, d);

    CarregaModelos();
    CriaPersonagens(10);
}

double nFrames = 0;
double TempoTotal = 0;

void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0 / 30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        angulo += 2;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: " << TempoTotal << " segundos. ";
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames / TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
    CriaBalasNavesInimigas();
}

//  trata o redimensionamento da janela OpenGL
void reshape(int w, int h)
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x, Max.x, Min.y, Max.y, -10, +10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void DesenhaDisparador()
{
    glPushMatrix();
    defineCor(DarkPurple);
    glPushMatrix();
    Disparador.desenhaPoligono();
    Disparador.pintaPoligono();
    glScaled(-1, 1, 1);           // Escala o polígono por -1 no eixo x
    Disparador.desenhaPoligono(); // Desenha o polígono espelhado
    Disparador.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

void DesenhaNaveInimiga()
{
    glPushMatrix();
    defineCor(Copper);
    glPushMatrix();
    NaveInimiga.desenhaPoligono();
    NaveInimiga.pintaPoligono();
    glScaled(-1, 1, 1);
    NaveInimiga.desenhaPoligono();
    NaveInimiga.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

void DesenhaBala()
{
    glPushMatrix();
    defineCor(BlueViolet);
    glPushMatrix();
    Bala.desenhaPoligono();
    Bala.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

float distanciaEntrePontos(const Ponto &p1, const Ponto &p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

Ponto CriaPontosAleatorios()
{
    float minX = Min.x + 10; // Evita a borda da área de jogo
    float maxX = Max.x - 10; // Evita a borda da área de jogo
    float minY = Min.y + 10; // Evita a borda da área de jogo
    float maxY = Max.y - 10; // Evita a borda da área de jogo

    Ponto novoPonto;
    bool pontoValido = false;

    while (!pontoValido)
    {
        float randX = minX + (rand() % (int)(maxX - minX));
        float randY = minY + (rand() % (int)(maxY - minY));
        novoPonto = Ponto(randX, randY);
        for (int i = 0; i < nPontos; i++)
        {
            if (distanciaEntrePontos(novoPonto, vetorPontos[i]) > 20)
            {
                pontoValido = true;
            }
        }
    }
    return novoPonto;
}

void inicializaPontos()
{
    for (int i = 0; i < nInstanciasPersonagens; i++)
    {
        if (i == 0)
        {
            vetorPontos[i] = Ponto(0, 0);
            nPontos++;
        }
        else
        {
            vetorPontos[i] = CriaPontosAleatorios();
            nPontos;
        }
    }
}

bool verificaBalaSaiuDaTela(Balas b)
{
    if (b.Posicao.y > Max.y || b.Posicao.y < Min.y || b.Posicao.x > Max.x || b.Posicao.x < Min.x)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool verificaBalaAtingiuObjeto(Balas b, float minX, float maxX, float minY, float maxY)
{
    if (b.Posicao.x >= minX && b.Posicao.x <= maxX && b.Posicao.y >= minY && b.Posicao.y <= maxY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void decrementaVidas()
{
    vidasDisparador--;
    // Verifica se o disparador foi destruído
    if (vidasDisparador == 0)
    {
        // Remove o disparador do array de personagens
        vetorDePersonagens[0] = vetorDePersonagens[nInstanciasPersonagens - 1];
        nInstanciasPersonagens--;
    }
}

void CriaPersonagens(int numeroDePersonagens)
{
    nInstanciasPersonagens = numeroDePersonagens;

    inicializaPontos();

    for (int i = 0; i < numeroDePersonagens; i++)
    {
        if (i == 0)
        {
            // caracteristicas apenas do disparador
            vetorDePersonagens[0].modelo = DesenhaDisparador;
            vetorDePersonagens[0].Largura = 9;
            vetorDePersonagens[0].Altura = 7;

            // caracteristicas gerais
            vetorDePersonagens[0].Posicao = vetorPontos[0];
            vetorDePersonagens[0].Rotacao = 0;
        }
        else
        {
            vetorDePersonagens[i].Posicao = vetorPontos[i];
            vetorDePersonagens[i].Rotacao = 0;
            vetorDePersonagens[i].modelo = DesenhaNaveInimiga;
            vetorDePersonagens[i].Largura = 6;
            vetorDePersonagens[i].Altura = 8;
        }
    }
}

void CriaBalasDisparador()
{
    if (vetorDePersonagens[0].nInstanciasBalas < 10)
    {
        // calcula a posição inicial da bala na ponta do disparador
        float deslocX = sin((vetorDePersonagens[0].Rotacao * M_PI) / 180);
        float deslocY = cos((vetorDePersonagens[0].Rotacao * M_PI) / 180);
        Ponto posicaoInicial = vetorDePersonagens[0].Posicao + Ponto(-deslocX * vetorDePersonagens[0].Largura / 2, deslocY * vetorDePersonagens[0].Altura);

        vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas].Posicao = posicaoInicial;
        vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas].Rotacao = vetorDePersonagens[0].Rotacao;
        vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas].modelo = DesenhaBala;

        // define o deslocamento da bala de acordo com a rotação do personagem
        vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas].Deslocamento.x = -deslocX;
        vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas].Deslocamento.y = deslocY;

        vetorDePersonagens[0].nInstanciasBalas++;
    }
}

void CriaBalasNavesInimigas()
{
    for (int i = 1; i < nInstanciasPersonagens; i++)
    {
        Ponto p1 = vetorDePersonagens[i].Posicao;
        Ponto p2 = vetorDePersonagens[0].Posicao;
        // Calcula o ângulo entre a posição da nave inimiga e a posição do disparador
        float anguloRad = atan2(p2.y - p1.y, p2.x - p1.x);

        // Converte o ângulo de radianos para graus
        float anguloGraus = anguloRad * 180.0 / M_PI;

        // Define o ângulo calculado como a rotação da nave inimiga
        vetorDePersonagens[i].Rotacao = anguloGraus;
        
        if (vetorDePersonagens[i].nInstanciasBalas < 10)
        {
            // calcula a posição inicial da bala na ponta do disparador
            float deslocX = sin((vetorDePersonagens[i].Rotacao * M_PI) / 180);
            float deslocY = cos((vetorDePersonagens[i].Rotacao * M_PI) / 180);
            Ponto posicaoInicial = vetorDePersonagens[i].Posicao + Ponto(-deslocX * vetorDePersonagens[i].Largura / 2, deslocY * vetorDePersonagens[i].Altura);

            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].Posicao = posicaoInicial;
            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].Rotacao = vetorDePersonagens[i].Rotacao;
            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].modelo = DesenhaBala;

            // define o deslocamento da bala de acordo com a rotação do personagem
            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].Deslocamento.x = -deslocX;
            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].Deslocamento.y = deslocY;

            vetorDePersonagens[i].nInstanciasBalas++;
        }
    }
}

void DesenhaPersonagens(float tempoDecorrido)
{
    for (int i = 0; i < nInstanciasPersonagens; i++)
    {
        if (i != 1)
        {
            vetorDePersonagens[i].AtualizaPosicao(tempoDecorrido);
        }
        vetorDePersonagens[i].desenha();
    }
}

void DesenhaBalasDisparador(float tempoDecorrido)
{
    for (int i = 0; i < vetorDePersonagens[0].nInstanciasBalas; i++)
    {
        Balas *b = &vetorDePersonagens[0].vetorDeBalas[i];
        b->Posicao.x += 3 * b->Deslocamento.x;
        b->Posicao.y += 3 * b->Deslocamento.y;

        b->AtualizaPosicao(tempoDecorrido);
        b->desenha();

        // verifica se a bala saiu da tela
        if (verificaBalaSaiuDaTela(vetorDePersonagens[0].vetorDeBalas[i]))
        {
            // remove a bala atual do array de balas
            vetorDePersonagens[0].vetorDeBalas[i] = vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas - 1];
            vetorDePersonagens[0].nInstanciasBalas--;
            i--;
        }

        // verifica se a bala atingiu uma nave inimiga
        for (int j = 1; j < nInstanciasPersonagens; j++)
        {
            // calcula os limites da área ocupada pela nave inimiga
            float minX = vetorDePersonagens[j].Posicao.x - vetorDePersonagens[j].Largura;
            float maxX = vetorDePersonagens[j].Posicao.x + vetorDePersonagens[j].Largura;
            float minY = vetorDePersonagens[j].Posicao.y;
            float maxY = vetorDePersonagens[j].Posicao.y + vetorDePersonagens[j].Altura;

            // verifica se a posição da bala está dentro dos limites da nave inimiga
            if (verificaBalaAtingiuObjeto(vetorDePersonagens[0].vetorDeBalas[i], minX, maxX, minY, maxY))
            {
                // remove a nave atingida do array de personagens
                vetorDePersonagens[j] = vetorDePersonagens[nInstanciasPersonagens - 1];
                nInstanciasPersonagens--;

                vetorDePersonagens[0].vetorDeBalas[i] = vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas - 1];
                vetorDePersonagens[0].nInstanciasBalas--;
                i--;
                break;
            }
        }
    }
}

void DesenhaBalasNavesInimigas(float tempoDecorrido)
{
    Ponto direcaoDisparador = vetorDePersonagens[0].Posicao;

    for (int i = 1; i < nInstanciasPersonagens; i++)
    {
        for (int j = 0; j < vetorDePersonagens[i].nInstanciasBalas; j++)
        {
            Balas *b = &vetorDePersonagens[i].vetorDeBalas[j];

            b->Posicao.x += 3 * b->Deslocamento.x;
            b->Posicao.y += 3 * b->Deslocamento.y;

            b->AtualizaPosicao(tempoDecorrido);
            b->desenha();
            // verifica se a bala saiu da tela
            if (verificaBalaSaiuDaTela(vetorDePersonagens[i].vetorDeBalas[j]))
            {
                // Remova a bala atual do array de balas da nave inimiga
                vetorDePersonagens[i].vetorDeBalas[j] = vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas - 1];
                vetorDePersonagens[i].nInstanciasBalas--;
                j--;
                break;
            }

            // verifica se a bala acertou o disparador
            float minX = vetorDePersonagens[0].Posicao.x - vetorDePersonagens[0].Largura;
            float maxX = vetorDePersonagens[0].Posicao.x + vetorDePersonagens[0].Largura;
            float minY = vetorDePersonagens[0].Posicao.y;
            float maxY = vetorDePersonagens[0].Posicao.y + vetorDePersonagens[0].Altura;

            if (verificaBalaAtingiuObjeto(vetorDePersonagens[i].vetorDeBalas[j], minX, maxX, minY, maxY))
            {
                // Remova a bala atual do array de balas da nave inimiga
                vetorDePersonagens[i].vetorDeBalas[j] = vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas - 1];
                vetorDePersonagens[i].nInstanciasBalas--;
                j--;

                decrementaVidas();
                break;
            }
        }
    }
}

void display(void)
{

    // Limpa a tela coma cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // Coloque aqui as chamadas das rotinas que desenham os objetos
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    glLineWidth(1);
    defineCor(SkyBlue);

    DesenhaPersonagens(T2.getDeltaT());
    DesenhaBalasDisparador(T2.getDeltaT());
    DesenhaBalasNavesInimigas(T2.getDeltaT());

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{

    switch (key)
    {
    case 27:     // Termina o programa qdo
        exit(0); // a tecla ESC for pressionada
        break;
    case ' ':
        if (vetorDePersonagens[0].nInstanciasBalas < 10)
        {
            CriaBalasDisparador();
        }
        break;
    default:
        break;
    }
}

void arrow_keys(int a_keys, int x, int y)
{
    switch (a_keys)
    {
    case GLUT_KEY_LEFT:
        vetorDePersonagens[0].Rotacao += 8;
        break;
    case GLUT_KEY_RIGHT:
        vetorDePersonagens[0].Rotacao -= 8;
        break;
    case GLUT_KEY_UP:
        if (vetorDePersonagens[0].Posicao.y + 1 < Max.y - 3)
        {
            // calcula o deslocamento do personagem de acordo com sua rotacao
            float deslocX = sin((vetorDePersonagens[0].Rotacao * M_PI) / 180);
            float deslocY = cos((vetorDePersonagens[0].Rotacao * M_PI) / 180);

            vetorDePersonagens[0].Posicao.x += 3 * -deslocX;
            vetorDePersonagens[0].Posicao.y += 3 * deslocY;
        }
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    cout << "Programa OpenGL" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize(650, 650);

    // Cria a janela na tela, definindo o nome da que aparecera na barra de t�tulo da janela.
    glutCreateWindow("Transformacoes Geometricas em OpenGL");

    // executa algumas inicializa��es
    init();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc(display);

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Sera chamada sempre que o usuario alterar a janela
    glutReshapeFunc(reshape);

    // Sera chamada sempre que o usuario pressionar uma tecla comum
    glutKeyboardFunc(keyboard);

    // Sera chamada sempre que o usuario pressionar as setas
    glutSpecialFunc(arrow_keys);

    // inicia o tratamento dos eventos
    glutMainLoop();

    return 0;
}
