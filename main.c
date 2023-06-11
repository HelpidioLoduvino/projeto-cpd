#include <stdio.h>
#include <stdlib.h>
#include "Docs-serial.h"
#include "Docs-serial.c"

main(){
	Documento *documento;
	Armario *armario;
	documento = ler_file();
	armario = armario_inicial(armario, documento);
	armario = ordenar_armario(documento);
}
