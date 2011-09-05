/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "pixasso-app.h"

int main (int argc, char **argv)
{
    PixassoApp *application = new PixassoApp (argc, argv);
    delete application;

    return 0;
}
