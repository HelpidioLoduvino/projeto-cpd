typedef struct documento Documento;
typedef struct armario Armario;

Documento *ler_file();
Armario *calcular_media_assunto(Armario *armario, Documento *documento);
Armario *calcular_distancia(Armario *armario, Documento *documento);
Armario *armario_inicial(Armario *armario, Documento *documento);
Armario *ordenar_armario(Documento *documento);
