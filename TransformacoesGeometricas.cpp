#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <vector>

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

Poligono Mapa, Disparador, NaveInimiga1, NaveInimiga2, NaveInimiga3, Bala;

// Limites logicos da area de desenho
Ponto Min, Max;

//define numero de personagens
int nInstanciasPersonagens = 10;

// Vetor para guardar as posições dos personagens
std::vector<Ponto> vetorPontos(nInstanciasPersonagens);

// Vetor para armazenar os personagens
std::vector<Personagens> vetorDePersonagens(nInstanciasPersonagens);

int nPontos = 0;

// caracteristicas inerentes ao disparador
int nInstanciasBalas = 0;
int vidasDisparador = 3;
float angulo = 0.0;

// variaveis de estado
bool telaInicial = true;
bool telaFinal = false;
bool telaVitoria = false;

void CriaPersonagens(int numeroDePersonagens);
void CriaBalasNavesInimigas();

void CarregaModelos()
{
    Disparador.LePoligono("Disparador.txt");
    NaveInimiga1.LePoligono("NaveInimiga1.txt");
    NaveInimiga2.LePoligono("NaveInimiga2.txt");
    NaveInimiga3.LePoligono("NaveInimiga3.txt");
    Bala.LePoligono("Bala.txt");
}

void init()
{
    float d = 150;
    Min = Ponto(-d, -d);
    Max = Ponto(d, d);

    CarregaModelos();
    CriaPersonagens(nInstanciasPersonagens);
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
    defineCor(NavyBlue);
    glPushMatrix();
    Disparador.desenhaPoligono();
    Disparador.pintaPoligono();
    glScaled(-1, 1, 1);
    Disparador.desenhaPoligono();
    Disparador.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

void DesenhaNaveInimiga1()
{
    glPushMatrix();
    defineCor(OrangeRed);
    glPushMatrix();
    NaveInimiga1.desenhaPoligono();
    NaveInimiga1.pintaPoligono();
    glScaled(-1, 1, 1);
    NaveInimiga1.desenhaPoligono();
    NaveInimiga1.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

void DesenhaNaveInimiga2()
{
    glPushMatrix();
    defineCor(OrangeRed);
    glPushMatrix();
    NaveInimiga2.desenhaPoligono();
    NaveInimiga2.pintaPoligono();
    glScaled(-1, 1, 1);
    NaveInimiga2.desenhaPoligono();
    NaveInimiga2.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

void DesenhaNaveInimiga3()
{
    glPushMatrix();
    defineCor(OrangeRed);
    glPushMatrix();
    NaveInimiga3.desenhaPoligono();
    NaveInimiga3.pintaPoligono();
    glScaled(-1, 1, 1);
    NaveInimiga3.desenhaPoligono();
    NaveInimiga3.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

void DesenhaBalaDisparador()
{
    glPushMatrix();
    defineCor(SkyBlue);
    glPushMatrix();
    Bala.desenhaPoligono();
    Bala.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

void DesenhaBalaNaveInimiga()
{
    glPushMatrix();
    defineCor(Firebrick);
    glPushMatrix();
    Bala.desenhaPoligono();
    Bala.pintaPoligono();
    glPopMatrix();
    glPopMatrix();
}

void DesenhaTelaInicial()
{
    glClear(GL_COLOR_BUFFER_BIT);

    defineCor(NavyBlue);
    glRasterPos2f(-50, 20);
    string texto = "Press SPACE to start";
    for (const char &c : texto)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    DesenhaDisparador();
}

void DesenhaTelaFinal()
{
    glClear(GL_COLOR_BUFFER_BIT);

    defineCor(OrangeRed);
    glRasterPos2f(-35, 20);
    string texto1 = "GAME OVER";
    for (const char &c : texto1)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2f(-22, -10);
    string texto2 = "Press R to restart";
    for (const char &c : texto2)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    glPushMatrix();
    glTranslatef(-15, 0, 0);
    DesenhaNaveInimiga1();
    glTranslatef(15, 0, 0);
    DesenhaNaveInimiga2();
    glTranslatef(15, 0, 0);
    DesenhaNaveInimiga3();
    glPopMatrix();
}

void DesenhaTelaVitoria()
{
    glClear(GL_COLOR_BUFFER_BIT);

    defineCor(NavyBlue);
    glRasterPos2f(-20, 20);
    string texto = "You WON!";
    for (const char &c : texto)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    DesenhaDisparador();
}

float distanciaEntrePontos(const Ponto &p1, const Ponto &p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

Ponto CriaPontosAleatorios()
{
    // Evita a borda da área de jogo
    Ponto min = Ponto(Min.x + 5, Min.y + 5);
    Ponto max = Ponto(Max.x - 5, Max.y - 5);

    // Evita o quadrado inicial do disparador
    Ponto minD = Ponto(vetorDePersonagens[0].Posicao.x -40,vetorDePersonagens[0].Posicao.y -40);
    Ponto maxD = Ponto(vetorDePersonagens[0].Posicao.x + 40, vetorDePersonagens[0].Posicao.y + 40);    

    Ponto novoPonto;
    bool pontoValido = false;

    while (!pontoValido)
    {
        float randX = min.x + (rand() % (int)(max.x - min.x));
        float randY = min.y + (rand() % (int)(max.y - min.y));
        novoPonto = Ponto(randX, randY);
        
        // Verifica se o ponto está dentro do quadrado inicial do disparador
        if (novoPonto.x >= minD.x && novoPonto.x <= maxD.x && novoPonto.y >= minD.y && novoPonto.y <= maxD.y)
        {
            continue; 
        }
        
        // Verifica se o ponto está a uma distância segura dos pontos existentes
        bool distanciaSegura = true;
        for (int i = 0; i < nPontos; i++)
        {
            if (distanciaEntrePontos(novoPonto, vetorPontos[i]) < 10)
            {
                distanciaSegura = false;
                break; 
            }
        }
        
        if (distanciaSegura)
        {
            pontoValido = true;
        }
    }
    return novoPonto;
}

void inicializaPontos(int nroPontos)
{
    for (int i = 0; i < nroPontos; i++)
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

void CriaPersonagens(int numeroDePersonagens)
{
    inicializaPontos(numeroDePersonagens);

    int div = numeroDePersonagens / 3;

    for (int i = 0; i < numeroDePersonagens; i++)
    {
        if (i == 0)
        {
            vetorDePersonagens[0].modelo = DesenhaDisparador;
            vetorDePersonagens[0].Largura = 9;
            vetorDePersonagens[0].Altura = 7;
            vetorDePersonagens[0].Posicao = vetorPontos[0];
            vetorDePersonagens[0].Rotacao = 0;
        }
        else if (i <= div)
        {
            vetorDePersonagens[i].Posicao = vetorPontos[i];
            vetorDePersonagens[i].Rotacao = 0;
            vetorDePersonagens[i].modelo = DesenhaNaveInimiga1;
            vetorDePersonagens[i].Largura = 6;
            vetorDePersonagens[i].Altura = 8;
        }
        else if (i <= (2 * div))
        {
            vetorDePersonagens[i].Posicao = vetorPontos[i];
            vetorDePersonagens[i].Rotacao = 0;
            vetorDePersonagens[i].modelo = DesenhaNaveInimiga2;
            vetorDePersonagens[i].Largura = 5;
            vetorDePersonagens[i].Altura = 8;
        }
        else if (i <= (3 * div))
        {
            vetorDePersonagens[i].Posicao = vetorPontos[i];
            vetorDePersonagens[i].Rotacao = 0;
            vetorDePersonagens[i].modelo = DesenhaNaveInimiga3;
            vetorDePersonagens[i].Largura = 4;
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
        vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas].modelo = DesenhaBalaDisparador;

        // define o deslocamento da bala de acordo com a rotação do personagem
        vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas].Deslocamento.x = -deslocX;
        vetorDePersonagens[0].vetorDeBalas[vetorDePersonagens[0].nInstanciasBalas].Deslocamento.y = deslocY;

        vetorDePersonagens[0].nInstanciasBalas++;
    }
}

void ApontaNavesDisparador(Personagens &p)
{
    Ponto pontoDisparador = vetorDePersonagens[0].Posicao;
    Ponto pontoNave = p.Posicao;

    // Calcula o vetor entre a nave inimiga e o disparador
    float vetorX = pontoDisparador.x - pontoNave.x;
    float vetorY = pontoDisparador.y - pontoNave.y;

    // Calcula o ângulo entre os vetores
    float anguloRad = atan(vetorY / vetorX);
    float anguloGraus = anguloRad * 180.0 / M_PI;

    if (pontoDisparador.x < pontoNave.x)
    {
        p.Rotacao = anguloGraus + 90;
    }
    if (pontoDisparador.x > pontoNave.x)
    {
        p.Rotacao = anguloGraus - 90;
    }
}

void CriaBalasNavesInimigas()
{
    for (int i = 1; i < nInstanciasPersonagens; i++)
    {
        ApontaNavesDisparador(vetorDePersonagens[i]);
        if (vetorDePersonagens[i].nInstanciasBalas < 10)
        {
            // calcula a posição inicial da bala na ponta do disparador
            float deslocX = sin((vetorDePersonagens[i].Rotacao * M_PI) / 180);
            float deslocY = cos((vetorDePersonagens[i].Rotacao * M_PI) / 180);
            Ponto posicaoInicial = vetorDePersonagens[i].Posicao + Ponto(-deslocX * vetorDePersonagens[i].Largura / 2, deslocY * vetorDePersonagens[i].Altura);

            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].Posicao = posicaoInicial;
            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].Rotacao = vetorDePersonagens[i].Rotacao;
            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].modelo = DesenhaBalaNaveInimiga;

            // define o deslocamento da bala de acordo com a rotação do personagem
            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].Deslocamento.x = -0.5 * deslocX;
            vetorDePersonagens[i].vetorDeBalas[vetorDePersonagens[i].nInstanciasBalas].Deslocamento.y = 0.5 * deslocY;

            vetorDePersonagens[i].nInstanciasBalas++;
        }
    }
}

void DesenhaPersonagens(float tempoDecorrido)
{
    for (int i = 0; i < nInstanciasPersonagens; i++)
    {
        if (i != 0)
        {
            vetorDePersonagens[i].AtualizaPosicao(tempoDecorrido, Min, Max);
        }
        vetorDePersonagens[i].desenha();
    }
}

void DesenhaBalasDisparador(float tempoDecorrido)
{
    for (int i = 0; i < vetorDePersonagens[0].nInstanciasBalas; i++)
    {
        Balas *b = &vetorDePersonagens[0].vetorDeBalas[i];

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

                if (nInstanciasPersonagens == 1)
                {
                    telaVitoria = true;
                }

                break;
            }
        }
    }
}

void DesenhaBalasNavesInimigas(float tempoDecorrido)
{
    for (int i = 1; i < nInstanciasPersonagens; i++)
    {
        for (int j = 0; j < vetorDePersonagens[i].nInstanciasBalas; j++)
        {
            Balas *b = &vetorDePersonagens[i].vetorDeBalas[j];

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

                vidasDisparador--; // Decrementa a vida da nave

                // Verifica se as vidas da nave acabaram
                if (vidasDisparador <= 0)
                {
                    telaFinal = true;
                }
                break;
            }
        }
    }
}

void DesenhaIconesVida()
{
    glColor3f(1.0, 0.0, 0.0); // Cor vermelha para os ícones de vida

    // Define a posição inicial dos ícones de vida no canto inferior esquerdo
    float posX = Min.x + 20;
    float posY = Min.y + 20;

    // Desenha os ícones de vida
    for (int i = 0; i < vidasDisparador; i++)
    {
        glPushMatrix();
        glTranslatef(posX, posY, 0.0);
        glBegin(GL_POLYGON);
        glVertex2f(0.0, 0.0);
        glVertex2f(10.0, 0.0);
        glVertex2f(10.0, 10.0);
        glVertex2f(0.0, 10.0);
        glEnd();
        glPopMatrix();

        // Ajusta a posição vertical para a próxima linha de ícones
        posY += 15.0;
    }
}

void AtualizaJogo(float temporizador){
    DesenhaPersonagens(T2.getDeltaT());
    DesenhaBalasDisparador(T2.getDeltaT());
    DesenhaBalasNavesInimigas(T2.getDeltaT());
    DesenhaIconesVida();
}

void movimentaDisparador()
{
    // calcula o deslocamento do personagem de acordo com sua rotacao
    float deslocX = sin((vetorDePersonagens[0].Rotacao * M_PI) / 180);
    float deslocY = cos((vetorDePersonagens[0].Rotacao * M_PI) / 180);

    Ponto novaPosicao = Ponto(vetorDePersonagens[0].Posicao.x + (3 * -deslocX), vetorDePersonagens[0].Posicao.y + (3 * deslocY));

    if (novaPosicao.y < Max.y && novaPosicao.y > Min.y && novaPosicao.x > Min.x && novaPosicao.x < Max.x)
    {
        vetorDePersonagens[0].Posicao.x = novaPosicao.x;
        vetorDePersonagens[0].Posicao.y = novaPosicao.y;
    }
}

void display(void)
{
    // Limpa a tela coma cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    if (telaInicial)
    {
        DesenhaTelaInicial();
    }
    else
    {
        if (telaVitoria)
        {
            DesenhaTelaVitoria();
        }
        else
        {
            if (telaFinal)
            {
                DesenhaTelaFinal();
            }
            else
            {
                AtualizaJogo(T2.getDeltaT());
            }
        }
    }
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
        if (telaInicial)
        {
            telaInicial = false;
        }
        if (vetorDePersonagens[0].nInstanciasBalas < 10)
        {
            CriaBalasDisparador();
        }
        break;
    case 'r':
        if (telaFinal)
        {
            nInstanciasBalas = 0;
            vidasDisparador = 3;
            angulo = 0.0;
            nPontos = 0;
            nInstanciasPersonagens = 10;
            Personagens p;
            for (int i = 0; i < nInstanciasPersonagens; i++)
            {
                vetorDePersonagens[i] = p;
            }
            init();
            telaInicial = true;
        }
        telaFinal = false;
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
        movimentaDisparador();
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
