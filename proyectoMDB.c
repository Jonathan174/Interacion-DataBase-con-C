//Jonathan Oswualdo Cuevas Jiménez 0225174

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <mysql/mysql.h>

#define DATABASE_NAME  "proyecto"
#define DATABASE_USERNAME "root"
#define DATABASE_PASSWORD "raspberry"


int mysql_connect (void);
void mysql_disconnect (void);
int mysql_read(void);
int mysql_write(void);
int hacerLogin(void);
void registro(void);
void agregarLED(char[]);
int verUsuario(void);
void controlLED(char[]);
int estadoLED(char[]);
MYSQL *mysql1;

int main(int argc, char **argv)
{
    int op = -1;
    if(mysql_connect()<0)
    {
		return -1;
    }
    
    while (op!=4)
    {
	    printf("1. Hacer login  \n");
	    printf("2. Registrar usuario \n");
	    printf("3. Ver usuario \n");
	    printf("4. Salir \n");
	    
	    scanf("%i", &op);
	    
	    if(op==1){
		hacerLogin();
	    }
	    else if(op==2){
		registro();
	    }
	    else if(op==3){
		verUsuario();
	    }
	    else if(op==4){
		mysql_disconnect();
		break;
	    }
	    else{
		printf("Opción no disponible, elige una opcion valida \n");
	    }
    }
    return 0;
}

int mysql_connect (void)
{
    //initialize MYSQL object for connections
	mysql1 = mysql_init(NULL);

    if(mysql1 == NULL)
    {
        printf("Error: %s\n", mysql_error(mysql1));
        return -1;
    }

    //Connect to the database
    if(mysql_real_connect(mysql1, "localhost", DATABASE_USERNAME, DATABASE_PASSWORD, DATABASE_NAME, 0, NULL, 0) == NULL)
    {
    	printf("Error:%s\n", mysql_error(mysql1));
    	return -1;
    }
    else
    {
        printf("Database connection successful.\n");
    }
    
    return 1;
}

void mysql_disconnect (void)
{
    mysql_close(mysql1);
    printf( "Disconnected from database.\n");
}


int hacerLogin(void){
    bool disponible=false;
    int num_rows=0;
    char correo[100]; 
    char contra[100];
    printf("Usuario: \n");
    scanf("%s", correo);
    printf("Contraseña: \n");
    scanf("%s", contra);
    char encrip[100];
    memset(encrip,0,100);
    for(int i=0;i<sizeof(contra)-1;i++){
	if(contra[i] != NULL)
	{
	    int aux=contra[i]-'0';
	    aux+=3;
	    encrip[i]=(aux+'0');
	}
    }
    if (!mysql_query(mysql1, "SELECT * FROM verificar"))
    {
	    MYSQL_RES *result = mysql_store_result(mysql1);
	    if (result != NULL)
	    {
		    //Get the number of columns
		    num_rows = mysql_num_rows(result);
		    int num_fields = mysql_num_fields(result);

		    MYSQL_ROW row;			//An array of strings
		    while( (row = mysql_fetch_row(result)) )
		    {
			    if(num_fields >= 3)
			    {
				    char *value_id = row[0];
				    char *value_nombre = row[1];
				    char *value_contra = row[2];
				    if((strcmp(value_nombre, correo) == 0) && (strcmp(value_contra, encrip) == 0))
				    {
					disponible=true;
					controlLED(value_id);
				    }    
			    }
		    }
		    if (disponible==false)
		    {
			printf("Usuario o contraseña incorrectos\n\n");
		    }
		mysql_free_result(result);
	    }
    }
    return num_rows;
}

void controlLED(char id[]){
    int op = -1;
    printf("BIENVENIDO  \n");
    while (op!=4)
    {
	
	printf("1. Encender/Apagar LED \n");
	printf("2. Ver estado del LED \n");
	printf("3. LogOut \n");
	
	scanf("%i", &op);
	
	if(op==1){
	    if(estadoLED(id) == 1)
	    {
		char query[200]="UPDATE estado SET valor='Apagado' WHERE id='";
		strcat(query, id);
		strcat(query, "'");
		
		if(mysql1 != NULL)
		{
		    if (mysql_query(mysql1, query))
		    {
			printf("Error %s\n", mysql_error(mysql1));
			
		    }
		}
		printf("Apagando LED\n\n");
	    }
	    
	    
	    else if(estadoLED(id) == 0)
	    {
		char query[200]="UPDATE estado SET valor='Encendido' WHERE id='";
		strcat(query, id);
		strcat(query, "'");
		if(mysql1 != NULL)
		{
		    if (mysql_query(mysql1, query))
		    {
			printf("Error %s\n", mysql_error(mysql1));
			
		    }
		}
		printf("Encendiendo LED\n\n");
	    }
	   
	}
	else if(op==2){
	    if(estadoLED(id)== 1)
	    {
		printf("Estado del LED Encendido\n\n");
	    }
	    
	    
	    else if(estadoLED(id)== 0)
	    {
		printf("Estado del LED Apagado\n\n");
	    }
	}
	else if(op==3){
	    break;
	}
	else{
	    printf("Opción no disponible, elige una opcion valida \n");
	}
    }
    

}

int estadoLED(char id[]){
    if(!mysql_query(mysql1, "SELECT * FROM estado"))
    {
	MYSQL_RES *result = mysql_store_result(mysql1);
	if (result != NULL)
	{
	    int num_fields = mysql_num_fields(result);

	    MYSQL_ROW row;			//An array of strings
	    while( (row = mysql_fetch_row(result)) )
	    {
		if(num_fields >= 3)
		{
		    char *value_id = row[0];
		    char *value_estado = row[2];
		    if(strcmp(value_id, id) == 0)
		    {
			if(strcmp(value_estado, "Apagado") == 0)
			{
			    return 0;
			    break;
			}
			else if(strcmp(value_estado, "Encendido") == 0)
			{
			    return 1;
			    break;
			} 
		    }
		}
	    }
	    mysql_free_result(result);
	}
    }
}



void registro(void){
    char correo[100]; 
    char contra[100];
    bool revisar=true;
    printf("Ingrese el usuario: \n");
    scanf("%s", correo);
    printf("Ingrese la contraseña: \n");
    scanf("%s", contra);
    char encrip[100];
    memset(encrip,0,100);
    for(int i=0;i<sizeof(contra)-1;i++){
	if(contra[i] != NULL)
	{
	    int aux=contra[i]-'0';
	    aux+=3;
	    encrip[i]=(aux+'0');
	}
    }
    if(!mysql_query(mysql1, "SELECT * FROM estado"))
    {
	MYSQL_RES *result = mysql_store_result(mysql1);
	if (result != NULL)
	{
	    int num_fields = mysql_num_fields(result);

	    MYSQL_ROW row;			//An array of strings
	    while( (row = mysql_fetch_row(result)) )
	    {
		if(num_fields >= 3)
		{
		    char *value_nombre = row[1];
		    if(strcmp(value_nombre, correo) == 0)
		    {
		    printf("Usuario ya existente, por favor vuelva a intentar \n\n");
		    revisar=false;
		    }
		}
	    }
	    if(revisar)
	    {
		char query[200]="INSERT INTO verificar (usuario, password) VALUES ('";
		strcat(query, correo);
		strcat(query, "', '");
		strcat(query, encrip);
		strcat(query, "')");
		
		if(mysql1 != NULL){
		    if (mysql_query(mysql1, query))
		    {
			printf("Error %s\n", mysql_error(mysql1));
		    }
		    else
		    {
			agregarLED(correo);
			printf("Registrado correctamente\n\n");
		    }
		}
	    }
	    mysql_free_result(result);
	}
    }
}

void agregarLED(char name[]){
    char status[]="Apagado";
    char query2[200]="INSERT INTO estado (usuario, valor) VALUES ('";
    strcat(query2, name);
    strcat(query2, "', '");
    strcat(query2, status);
    strcat(query2, "')");
    
    if(mysql1 != NULL){
        if (mysql_query(mysql1, query2))
        {
            printf("Error %s\n", mysql_error(mysql1));
        }
	else{
	    printf("LED enlazado\n");
	}
    }
}

int verUsuario(void){
    bool revisar=true;
    int num_rows=0;
    char correo[100]; 
    printf("Que usuario desea buscar: \n");
    scanf("%s", correo);

    if(!mysql_query(mysql1, "SELECT * FROM estado"))
    {
	MYSQL_RES *result = mysql_store_result(mysql1);
	if (result != NULL)
	{
	    //Get the number of columns
	    num_rows = mysql_num_rows(result);
	    int num_fields = mysql_num_fields(result);

	    MYSQL_ROW row;			//An array of strings
	    while( (row = mysql_fetch_row(result)) )
	    {
		if(num_fields >= 3)
		{
		    char *value_id = row[0];
		    char *value_nombre = row[1];
		    char *value_estado = row[2];
		    if(strcmp(value_nombre, correo) == 0)
		    {
		    printf("La información del usuario es: \n");
		    printf( "ID: %s, nombre: %s, y el estado de su LED está %s.\n\n", value_id, value_nombre, value_estado);
		    revisar=false;
		    }
		}
	    }
	    if(revisar)
	    {
		printf("El usuario no existe \n\n");
	    }
	    mysql_free_result(result);
	}
    }
    return num_rows;
}
