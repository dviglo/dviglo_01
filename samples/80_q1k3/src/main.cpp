// ������ ������
#include <SDL.h> // SDL_main

// ����������� ����������
#include <clocale> // setlocale

int SDL_main(int argc, char* argv[])
{
    // ��������� �������� UTF-8 � ������� � Windows. � Linux ������� � ��������� UTF-8 �� ���������
    setlocale(LC_CTYPE, "en_US.UTF-8");

    return 0;
}
