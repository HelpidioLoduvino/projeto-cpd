#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

typedef struct documento{
	int num_assunto, num_documento, num_armario, indice_documento, armario_principal, posicao_armario;
	double menor_distancia, *assunto;
}Documento;

typedef struct armario{
	Documento *documento;
	int num_armario, quantidade_documento;
	double *media_assunto;
}Armario;

Documento *ler_file(){
	int i = 0, num_documento, num_armario, num_assunto, j=0, k=0;
	Documento *documento;
	Armario *armario;
	
	float pontuacao_assunto;
	FILE *file = fopen("docs.in.txt", "r");
	if(file == NULL)
		printf("\n Erro!");
	else{
		while(fscanf(file, "%f", &pontuacao_assunto) == 1){
			if(i == 0){
				num_armario = pontuacao_assunto;
				i++;
			}
			else if(i == 1){
				num_documento = pontuacao_assunto;
				i++;
			}
			else if(i == 2){
				num_assunto = pontuacao_assunto;
				i++;
			} else
				break;
		}
		fclose(file);
		
	}
	
	float vet_documento [(num_documento*num_assunto)+num_documento+3];
	
	i = 0;
	file = fopen("docs.in.txt", "r");
	if(file == NULL)
		printf("\n Erro!");
	else{
		while(fscanf(file, "%f", &pontuacao_assunto) == 1){
			if(i > 2)	
				vet_documento[j] = pontuacao_assunto;
				
			i++;
			j++;
		}
		fclose(file);
	}
	
	documento = calloc(num_documento, sizeof(Documento));
	documento->num_assunto = num_assunto;
	documento->num_documento = num_documento;
	documento->num_armario = num_armario;
	
	for(i = 0; i < documento->num_documento; i ++){
		documento[i].assunto = calloc(num_assunto, sizeof(double));
		documento[i].num_assunto = num_assunto;
		documento[i].num_documento = num_documento;
		documento[i].num_armario = num_armario;
	}
	k = 3;
	
	for(i = 0; i < num_documento; i++){
		documento[i].indice_documento = vet_documento[k];
		k++;
		for(j = 0; j < num_assunto; j++){
			documento[i].assunto[j] = vet_documento[k];
			k++;
		}
	}
	return documento;
}

Armario *calcular_media_assunto(Armario *armario, Documento *documento){
	int i=0, j, k=0, l=0, cont = 0;
	double media=0;
	
		#pragma omp parallel for private(cont,j)
	while(cont < (documento->num_documento*documento->num_assunto)){
		for(j = 0; j < armario[i].quantidade_documento; j++){
			media = media + armario[i].documento[j].assunto[k];
			cont++;
			l++;
		}
		media = (media /armario[i].quantidade_documento);
		armario[i].media_assunto[k] = media;
		media = 0;
		k++;
		
		if((k) == documento->num_assunto)
			k=0;
			
		if(l == (armario[i].quantidade_documento*documento->num_assunto)){
			i++;
			l = 0;
		}
	}
	return armario;
}

Armario *calcular_distancia(Armario *armario, Documento *documento){
	int i=0, j=0, k=0, l=0, cont = 0;
	double distancia=0, matrizMediaAssunto[documento->num_armario][documento->num_assunto],
	matrizDistanciaDocumentoArmario[documento->num_documento][documento->num_armario];
	
	#pragma omp parallel for private(i,j,k)
	for(i = 0; i < documento->num_armario; i++){
		for(j = 0; j < armario[i].quantidade_documento; j++){
			for(k = 0; k < documento->num_documento; k++){
				if(armario[i].documento[j].indice_documento == documento[k].indice_documento){
					documento[k] = armario[i].documento[j];
					k = documento->num_documento-1;
				}
			}
		}
	}

	
	for(i = 0; i < documento->num_armario; i++){
		for(j = 0; j < documento->num_assunto; j++){
			matrizMediaAssunto[i][j] = armario[i].media_assunto[j];
		}
	}
	
	for(i = 0; i < documento->num_documento; i++){
		for(j = 0; j < documento->num_armario; j++){
			for(k = 0; k < documento->num_assunto; k++){
				distancia = distancia + pow((documento[i].assunto[k] - matrizMediaAssunto[j][k]), 2);
			}
			matrizDistanciaDocumentoArmario[i][j] = distancia;
			distancia = 0;
		}
		documento[i].menor_distancia = matrizDistanciaDocumentoArmario[i][documento->armario_principal];
	}
	
	for(i = 0; i < documento->num_documento; i++){
		for(j = 0; j < documento->num_armario; j++){
			if(documento[i].menor_distancia > matrizDistanciaDocumentoArmario[i][j]){
				documento[i].posicao_armario = j;
				documento[i].menor_distancia = matrizDistanciaDocumentoArmario[i][j];
			}
		}
	}

	return armario;
}

Armario *armario_inicial(Armario *armario, Documento *documento){
	int i = 0, j=0 , k= 0, calculo;
	armario = calloc(documento->num_armario, sizeof(Armario));
	armario->num_armario = documento->num_armario;
	
	for(i = 0; i < documento->num_armario; i++){
		armario[i].media_assunto = calloc(documento->num_assunto, sizeof(double));
		armario[i].documento = calloc(documento->num_documento, sizeof(Documento));
		armario[i].quantidade_documento = 0;
		for(j = 0; j < documento->num_documento; j++)
			armario[i].documento[j].assunto = calloc(documento->num_assunto, sizeof(double));
	}
	
	for(i = 0; i < documento->num_documento; i++){
		calculo = documento[i].indice_documento % documento->num_armario;
		armario[calculo].quantidade_documento++;
	}
	
	for(i = 0; i < armario->num_armario; i++){
		for(j = 0; j < documento->num_documento; j++){
			calculo = j % documento[j].num_armario;
			if(calculo == i){
				documento[j].armario_principal = calculo;
				armario[i].documento[k] = documento[j];
				k++;
			}
		}
		k = 0;
	}

	armario = calcular_media_assunto(armario, documento);
	armario = calcular_distancia(armario, documento);
	
	return armario;
}

Armario *ordenar_armario(Documento *documento){
	int i, j, k;
	Armario *armario;
	armario = calloc(documento->num_armario, sizeof(Armario));
	armario->num_armario = documento->num_armario;
	for(i = 0; i < documento->num_armario; i++){
		armario[i].media_assunto = calloc(documento->num_assunto, sizeof(double));
		armario[i].documento = calloc(documento->num_documento, sizeof(Documento));
		armario[i].quantidade_documento = 0;
	}
	
	for(i = 0; i < documento->num_armario; i++){
		for(j = 0; j < documento->num_documento; j++){
			if(documento[j].posicao_armario == i){
				armario[i].documento[armario[i].quantidade_documento] = documento[j];
				armario[i].quantidade_documento++;
			}
		}
	}
	
	for(i = 0; i < documento->num_armario; i++){
		for(j = 0; j < armario[i].quantidade_documento; j++)
			armario[i].documento[i].assunto = calloc(documento->num_assunto, sizeof(double));
	}
	
	FILE *file = fopen("docs.out.txt", "w");
	if(file == NULL)
		printf("Erro!");
	else{
		printf("Programa terminado com sucesso!");
		char a ='\n';
		for(i = 0; i < documento->num_documento; i++)
			fprintf(file, "%d %d%c", documento[i].indice_documento, documento[i].posicao_armario, a);
	}
	fclose(file);
	return armario;
}
