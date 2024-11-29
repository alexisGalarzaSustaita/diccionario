#include "dictionary.h"
//Inicializa el diccionario de datos
/*FILE* initializeDataDictionary(const char *dictionaryName) {
    long mainHeader = EMPTY_POINTER;

    printf("Initializing Data Dictionary...\n");

    FILE *dictionary = fopen(dictionaryName, "w+");

    fwrite(&mainHeader, sizeof(mainHeader), 1, dictionary);
        
    return dictionary;
}*/
FILE* initializeDataDictionary(const char *dictionaryName, const char *mode) {
    long mainHeader = EMPTY_POINTER;

    printf("\nInitializing Data Dictionary...\n");

    FILE *dictionary = fopen(dictionaryName, mode);

    if (dictionary == NULL) {
        fprintf(stderr, "Error opening file: %s\n", dictionaryName);
        return NULL;
    }

    fwrite(&mainHeader, sizeof(mainHeader), 1, dictionary);

    return dictionary;
}

//Agrega una nueva entidad
int appendEntity(FILE* dataDictionary, ENTITY newEntity) {
 
    fseek(dataDictionary, 0, SEEK_END);
    
    long entityDirection = ftell(dataDictionary);

    fwrite(newEntity.name, DATA_BLOCK_SIZE, 1, dataDictionary); 
    fwrite(&newEntity.dataPointer, sizeof(long), 1, dataDictionary); 
    fwrite(&newEntity.attributesPointer, sizeof(long), 1, dataDictionary);
    fwrite(&newEntity.nextEntity, sizeof(long), 1, dataDictionary); 

    return entityDirection; 
}
//Ordena las entidades
void reorderEntities(FILE* dataDictionary, long currentEntityPointer, const char* newEntityName, long newEntityDirection){
    long currentEntityDirection = -1; 

    fseek(dataDictionary, currentEntityPointer, SEEK_SET);
    fread(&currentEntityDirection, sizeof(currentEntityDirection), 1, dataDictionary); 

    if (currentEntityDirection == -1) {
        
        fseek(dataDictionary, currentEntityPointer, SEEK_SET);
        fwrite(&newEntityDirection, sizeof(long), 1, dataDictionary);
    }
    else {
        char currentEntityName[DATA_BLOCK_SIZE]; 
        long nextEtityDirection; 
        long nextHeaderPointer; 


        fseek(dataDictionary, currentEntityDirection, SEEK_SET);

        fread(&currentEntityName, sizeof(char), DATA_BLOCK_SIZE, dataDictionary);
        nextHeaderPointer = ftell(dataDictionary) + (sizeof(long) * 2);

        if (strcmp(currentEntityName, newEntityName) < 0) {
            
            reorderEntities(dataDictionary, nextHeaderPointer, newEntityName, newEntityDirection);
        }
        else {

            fseek(dataDictionary, currentEntityPointer, SEEK_SET);
            fwrite(&newEntityDirection, sizeof(long), 1, dataDictionary);

            fseek(dataDictionary, newEntityDirection + DATA_BLOCK_SIZE + (sizeof(long) * 2), SEEK_SET);
            fwrite(&currentEntityDirection, sizeof(long), 1, dataDictionary);
        }
    }
}
//Crea una nueva entidad
void createEntity(FILE* dataDictionary) { 
    ENTITY newEntity; 

    printf("\nEnter the Entity name: "); 
    fgets(newEntity.name, sizeof(newEntity.name), stdin); 
    newEntity.name[strcspn(newEntity.name, "\n")] = '\0';
    newEntity.dataPointer = EMPTY_POINTER;
    newEntity.attributesPointer = EMPTY_POINTER; 
    newEntity.nextEntity = EMPTY_POINTER; 

    long entityDirection = appendEntity(dataDictionary, newEntity);
    reorderEntities(dataDictionary, MAIN_ENTITY_POINTER, newEntity.name, entityDirection);
}

/*void createAttribute(FILE* dataDictionary, ENTITY currentEntity) {
    ATTRIBUTE newAttribute;
    long type;  

    printf("\nEnter the Attribute name: "); 
    fgets(newAttribute.name, sizeof(newAttribute.name), stdin);
    fflush(stdin); 
    printf("\nIs primary key? (true/false)");
    fgets(newAttribute.isPrimary, sizeof(bool), stdin); 
    printf("\nAttribute type: 1)int 2)long 3)float 4)char 5)bool")
    scanf("%ld", &type); 
    newAttribute.type = type;
    attributeSize(newAttribute);
    newAttribute.nextAttribute =EMPTY_POINTER;  

    long attributeDirection = appendAttribute(dataDictionary, newAttribute);
    reorderAttributes(dataDictionary, currentEntity.attributesPointer, newAttribute.name, attributeDirection);
}*/
//Crea un nuevo atributo
void createAttribute(FILE* dataDictionary, ENTITY currentEntity) {
    ATTRIBUTE newAttribute;
    long type;
    char response[10];

    fflush(stdin);
    printf("\nEnter the Attribute name: "); 
    fgets(newAttribute.name, sizeof(newAttribute.name), stdin);
    newAttribute.name[strcspn(newAttribute.name, "\n")] = '\0';
    fflush(stdin);
    printf("\nIs primary key? 0)false 1)true: ");
    fgets(response, sizeof(response), stdin);

    if(strcmp(response, "true") == 0)
        newAttribute.isPrimary = 1; 
    else
        newAttribute.isPrimary = 0; 
    fflush(stdin);
    printf("\nAttribute type: 1)int 2)long 3)float 4)char 5)bool: ");
    scanf("%ld", &type);
    newAttribute.type = type; 
    attributeSize(newAttribute);
    newAttribute.nextAttribute = EMPTY_POINTER;

    long attributeDirection = appendAttribute(dataDictionary, newAttribute);
    reorderAttributes(dataDictionary, currentEntity.attributesPointer, newAttribute.name, attributeDirection);
}
/*void createAttribute(FILE* dataDictionary, ENTITY currentEntity) {
    ATTRIBUTE newAttribute;
    long type;
    char response[10];

    fflush(stdin);
    printf("\nEnter the Attribute name: ");
    fgets(newAttribute.name, sizeof(newAttribute.name), stdin);
    newAttribute.name[strcspn(newAttribute.name, "\n")] = '\0';
    fflush(stdin);
    printf("\nIs primary key? 0)false 1)true: ");
    fgets(response, sizeof(response), stdin);

    newAttribute.isPrimary = (strcmp(response, "1") == 0);
    fflush(stdin);
    printf("\nAttribute type: 1)int 2)long 3)float 4)char 5)bool: ");
    scanf("%ld", &type);
    newAttribute.type = type;
    attributeSize(newAttribute);
    newAttribute.nextAttribute = EMPTY_POINTER;

    long attributeDirection = appendAttribute(dataDictionary, newAttribute);

    // Actualiza el puntero attributesPointer de la entidad
    reorderAttributes(dataDictionary, currentEntity.attributesPointer, newAttribute.name, attributeDirection);

    // Sobrescribe la entidad con el nuevo puntero actualizado
    fseek(dataDictionary, ftell(dataDictionary) - sizeof(ENTITY), SEEK_SET);
    fwrite(&currentEntity, sizeof(ENTITY), 1, dataDictionary);

    printf("\nAttribute '%s' added successfully to entity '%s'.\n", newAttribute.name, currentEntity.name);
}*/

//Aggrega el atributo a la entidad a la que pertenece
int appendAttribute(FILE* dataDictionary, ATTRIBUTE newAttribute) {
 
    fseek(dataDictionary, 0, SEEK_END);
    
    long attributeDirection = ftell(dataDictionary);

    fwrite(newAttribute.name, DATA_BLOCK_SIZE, 1, dataDictionary); 
    fwrite(&newAttribute.isPrimary, sizeof(bool), 1, dataDictionary); 
    fwrite(&newAttribute.type, sizeof(long), 1, dataDictionary);
    fwrite(&newAttribute.size, sizeof(long), 1, dataDictionary);
    fwrite(&newAttribute.nextAttribute, sizeof(long), 1, dataDictionary);  

    return attributeDirection; 
}
//Ordena los atributos de la entidad
/*void reorderAttributes(FILE* dataDictionary, long currentAttributePointer, const char* newAttributeName, long newAttributeDirection){
    long currentAttributeDirection = -1; 

    fseek(dataDictionary, currentAttributePointer, SEEK_SET);
    fread(&currentAttributeDirection, sizeof(currentAttributeDirection), 1, dataDictionary); 

    if (currentAttributeDirection == -1) {
        
        fseek(dataDictionary, currentAttributePointer, SEEK_SET);
        fwrite(&newAttributeDirection, sizeof(long), 1, dataDictionary);
    }
    else {
        char currentAttributeName[DATA_BLOCK_SIZE]; 
        long nextAttributeDirection; 
        long nextHeaderPointer; 


        fseek(dataDictionary, currentAttributeDirection, SEEK_SET);

        fread(&currentAttributeName, sizeof(char), DATA_BLOCK_SIZE, dataDictionary);
        nextHeaderPointer = ftell(dataDictionary) + sizeof(bool) + (sizeof(long) * 2);

        if (strcmp(currentAttributeName, newAttributeName) < 0) {
            
            reorderAttributes(dataDictionary, nextHeaderPointer, newAttributeName, newAttributeDirection);
        }
        else {

            fseek(dataDictionary, currentAttributePointer, SEEK_SET);
            fwrite(&newAttributeDirection, sizeof(long), 1, dataDictionary);

            fseek(dataDictionary, newAttributeDirection + DATA_BLOCK_SIZE + sizeof(bool) + (sizeof(long) * 2), SEEK_SET);
            fwrite(&currentAttributeDirection, sizeof(long), 1, dataDictionary);
        }
    }
}*/
void reorderAttributes(FILE* dataDictionary, long currentAttributePointer, const char* newAttributeName, long newAttributeDirection) {
    long currentAttributeDirection = -1;

    fseek(dataDictionary, currentAttributePointer, SEEK_SET);
    fread(&currentAttributeDirection, sizeof(currentAttributeDirection), 1, dataDictionary);

    if (currentAttributeDirection == -1) {
        fseek(dataDictionary, currentAttributePointer, SEEK_SET);
        fwrite(&newAttributeDirection, sizeof(long), 1, dataDictionary);
    } else {
        char currentAttributeName[DATA_BLOCK_SIZE];
        long nextAttributeDirection;
        long nextHeaderPointer;

        fseek(dataDictionary, currentAttributeDirection, SEEK_SET);

        fread(&currentAttributeName, sizeof(char), DATA_BLOCK_SIZE, dataDictionary);
        nextHeaderPointer = ftell(dataDictionary) + sizeof(bool) + (sizeof(long) * 2);

        if (strcmp(currentAttributeName, newAttributeName) < 0) {
            reorderAttributes(dataDictionary, nextHeaderPointer, newAttributeName, newAttributeDirection);
        } else {
            fseek(dataDictionary, currentAttributePointer, SEEK_SET);
            fwrite(&newAttributeDirection, sizeof(long), 1, dataDictionary);

            fseek(dataDictionary, newAttributeDirection + DATA_BLOCK_SIZE + sizeof(bool) + (sizeof(long) * 2), SEEK_SET);
            fwrite(&currentAttributeDirection, sizeof(long), 1, dataDictionary);
        }
    }
}

//Eliminar una entidad (Eliminacion lógica)
ENTITY removeEntity(FILE* dataDictionary, long currentEntityPointer, const char* entityName){
    long currentEntityDirection = -1; 

    fseek(dataDictionary, currentEntityDirection, SEEK_SET); 
    fread(&currentEntityDirection, sizeof(long), 1, dataDictionary); 

    if(currentEntityDirection == -1){
        ENTITY emptyEntity = {0};
        return emptyEntity; 
    }
    else{
        ENTITY resultEntity; 
        long nextEntityDirection; 
        long nextHeaderPointer; 

        fseek(dataDictionary, currentEntityDirection, SEEK_SET); 
        fread(resultEntity.name, sizeof(char), DATA_BLOCK_SIZE, dataDictionary);
        nextHeaderPointer = ftell(dataDictionary) + (sizeof(long) * 2); 

        if(strcmp(resultEntity.name, entityName) == 0){
            fread(&resultEntity.dataPointer, sizeof(long), 1, dataDictionary); 
            fread(&resultEntity.attributesPointer, sizeof(long), 1, dataDictionary); 
            fread(&resultEntity.nextEntity, sizeof(long), 1, dataDictionary); 

            fseek(dataDictionary, currentEntityPointer, SEEK_SET);
            fwrite(&resultEntity.nextEntity, sizeof(long), 1, dataDictionary); 

            return resultEntity;
        }
        else{
            return removeEntity(dataDictionary, nextHeaderPointer, entityName);
        } 
    }
}

//Asignar el tamaño del atributo
void attributeSize(ATTRIBUTE newAtribute){
    int number; 

    switch (newAtribute.type){
        case 1:
            newAtribute.size = sizeof(int); 
            break;

        case 2:
            newAtribute.size = sizeof(long); 
            break;

        case 3:
            newAtribute.size = sizeof(float); 
            break;

        case 4:
            printf("Enter string size: ");
            scanf("%d", &number); 
            fflush(stdin);
            newAtribute.size = sizeof(char) * number; 
            break; 

        case 5: 
            newAtribute.size = sizeof(bool); 
            break;

        default:
            printf("Invalid attribute type.\n");
            break;
    }
}

//Eliminar atributo
ATTRIBUTE removeAttribute(FILE* dataDictionary, long currentAttributePointer, const char* attributeName) {
    long currentAttributeDirection = -1;

    fseek(dataDictionary, currentAttributePointer, SEEK_SET);
    fread(&currentAttributeDirection, sizeof(long), 1, dataDictionary);

    if (currentAttributeDirection == -1) {
        ATTRIBUTE emptyAttribute = {0};
        return emptyAttribute;
    } else {
        ATTRIBUTE resultAttribute;
        long nextAttributeDirection;
        long nextHeaderPointer; 

        fseek(dataDictionary, currentAttributeDirection, SEEK_SET);
        fread(resultAttribute.name, sizeof(char), DATA_BLOCK_SIZE, dataDictionary);
        nextHeaderPointer = ftell(dataDictionary) + sizeof(bool) + (sizeof(long) * 2); 

        if (strcmp(resultAttribute.name, attributeName) == 0) {
            fread(&resultAttribute.isPrimary, sizeof(bool), 1, dataDictionary); 
            fread(&resultAttribute.type, sizeof(long), 1, dataDictionary);
            fread(&resultAttribute.size, sizeof(long), 1, dataDictionary);  
            fread(&resultAttribute.nextAttribute, sizeof(long), 1, dataDictionary);         

            fseek(dataDictionary, currentAttributePointer, SEEK_SET);
            fwrite(&resultAttribute.nextAttribute, sizeof(long), 1, dataDictionary);

            return resultAttribute;
        } else {
            return removeAttribute(dataDictionary, nextHeaderPointer, attributeName);
        }
    }
}

//Captura entidades
void captureEntities(FILE* dataDictionary) {
    int keepCapturing = 1;

    while (keepCapturing) {
        printf("\n--- Capturing Entity ---\n");
        createEntity(dataDictionary);

        printf("\nDo you want to add another entity? 1)Yes 0)No: ");
        scanf("%d", &keepCapturing);
        getchar();
    }
}

//Buscar entidad por nombre: muestra una lista de los nombres de las entidades para que el usuario escriba la que entidad que quiere buscar
ENTITY searchEntityByName(FILE* dataDictionary, const char* entityName) {
    long currentEntityDirection = -1;

    fseek(dataDictionary, MAIN_ENTITY_POINTER, SEEK_SET);
    fread(&currentEntityDirection, sizeof(currentEntityDirection), 1, dataDictionary);

    while (currentEntityDirection != EMPTY_POINTER) {
        ENTITY currentEntity;

        fseek(dataDictionary, currentEntityDirection, SEEK_SET);
        fread(currentEntity.name, DATA_BLOCK_SIZE, 1, dataDictionary);
        fread(&currentEntity.dataPointer, sizeof(long), 1, dataDictionary);
        fread(&currentEntity.attributesPointer, sizeof(long), 1, dataDictionary);
        fread(&currentEntity.nextEntity, sizeof(long), 1, dataDictionary);

        if (strcmp(currentEntity.name, entityName) == 0) {
            printf("\nEntity '%s' found.\n", currentEntity.name);
            return currentEntity; 
        }

        currentEntityDirection = currentEntity.nextEntity;
    }

    printf("\nEntity '%s' not found.\n", entityName);
    ENTITY emptyEntity = {0};
    return emptyEntity;
}

//Captura atributos para una entidad
void captureAttributes(FILE* dataDictionary, ENTITY currentEntity) {
    int keepCapturing = 1;

    printf("\n--- Capturing Attribute for Entity: %s ---\n", currentEntity.name);
    while (keepCapturing) {
        //printf("\n--- Capturing Attribute for Entity: %s ---\n", currentEntity.name);
        createAttribute(dataDictionary, currentEntity);

        printf("\nDo you want to add another attribute? 1)Yes 0)No: ");
        scanf("%d", &keepCapturing);
        getchar();
    }
}

//Funcion trampolin para buscar la entidad y agregar los atributos
void captureAttributesForEntity(FILE* dataDictionary) {
    char entityName[DATA_BLOCK_SIZE];

    showEntities(dataDictionary);

    printf("\nEnter the name of the entity to add attributes: ");
    fgets(entityName, sizeof(entityName), stdin);
    entityName[strcspn(entityName, "\n")] = '\0'; 

    ENTITY entity = searchEntityByName(dataDictionary, entityName);

    if (strcmp(entity.name, entityName) == 0) {
        captureAttributes(dataDictionary, entity);
    } else {
        printf("\nCannot add attributes. Entity not found.\n");
    }
}

//Muestra las entidades con los atributos
void showEntitiesWithAttributes(FILE* dataDictionary) {
    long currentEntityDirection = -1;

    fseek(dataDictionary, MAIN_ENTITY_POINTER, SEEK_SET);
    fread(&currentEntityDirection, sizeof(currentEntityDirection), 1, dataDictionary);

    if (currentEntityDirection == EMPTY_POINTER) {
        printf("\nNo entities found.\n");
        return;
    }

    printf("\n--- Entities List ---\n");
    while (currentEntityDirection != EMPTY_POINTER) {
        ENTITY currentEntity;

        fseek(dataDictionary, currentEntityDirection, SEEK_SET);
        fread(currentEntity.name, DATA_BLOCK_SIZE, 1, dataDictionary);
        fread(&currentEntity.dataPointer, sizeof(long), 1, dataDictionary);
        fread(&currentEntity.attributesPointer, sizeof(long), 1, dataDictionary);
        fread(&currentEntityDirection, sizeof(long), 1, dataDictionary);

        printf("\nEntity Name: %s\n", currentEntity.name);
        showAttributes(dataDictionary, currentEntity.attributesPointer);
    }
}

//Muestra los atributos de una entidad
void showAttributes(FILE* dataDictionary, long attributesPointer) {
    long currentAttributeDirection = attributesPointer;

    if (currentAttributeDirection == EMPTY_POINTER) {
        printf("\nNo attributes found for this entity.\n");
        return;
    }

    printf("\n--- Attributes List ---\n");
    while (currentAttributeDirection != EMPTY_POINTER) {
        ATTRIBUTE currentAttribute;

        fseek(dataDictionary, currentAttributeDirection, SEEK_SET);
        fread(currentAttribute.name, DATA_BLOCK_SIZE, 1, dataDictionary);
        fread(&currentAttribute.isPrimary, sizeof(bool), 1, dataDictionary);
        fread(&currentAttribute.type, sizeof(long), 1, dataDictionary);
        fread(&currentAttribute.size, sizeof(long), 1, dataDictionary);
        fread(&currentAttributeDirection, sizeof(long), 1, dataDictionary);

        printf("Name: %s\n", currentAttribute.name);
        printf("Primary Key: %s\n", currentAttribute.isPrimary ? "Yes" : "No");
        printf("Type: %ld\n", currentAttribute.type);
        printf("Size: %ld bytes\n", currentAttribute.size);
    }
}
/*void showAttributes(FILE* dataDictionary, long attributesPointer) {
    long currentAttributeDirection = attributesPointer;

    if (attributesPointer == EMPTY_POINTER) {
        printf("\nNo attributes found for this entity.\n");
        return;
    }

    printf("\n--- Attributes List ---\n");
    while (attributesPointer != EMPTY_POINTER) {
        ATTRIBUTE currentAttribute;

        fseek(dataDictionary, attributesPointer, SEEK_SET);

        // Lee la información del atributo
        fread(currentAttribute.name, DATA_BLOCK_SIZE, 1, dataDictionary);
        fread(&currentAttribute.isPrimary, sizeof(bool), 1, dataDictionary);
        fread(&currentAttribute.type, sizeof(long), 1, dataDictionary);
        fread(&currentAttribute.size, sizeof(long), 1, dataDictionary);
        fread(&attributesPointer, sizeof(long), 1, dataDictionary);

        printf("Name: %s\n", currentAttribute.name);
        printf("Primary Key: %s\n", currentAttribute.isPrimary ? " 1)Yes" : "0)No");
        printf("Type: %ld\n", currentAttribute.type);
        printf("Size: %ld bytes\n", currentAttribute.size);
    }
}*/



//Muestra los nombres de las entidades
/*void showEntities(FILE* dataDictionary) {
  long currentEntityDirection = -1;

  fseek(dataDictionary, MAIN_ENTITY_POINTER, SEEK_SET);
  fread(&currentEntityDirection, sizeof(currentEntityDirection), 1, dataDictionary);

  if (currentEntityDirection == EMPTY_POINTER) {
    printf("\nNo entities found.\n");
    return;
  }

  printf("\n--- Entities List ---\n");
  while (currentEntityDirection != EMPTY_POINTER) {
    ENTITY currentEntity;

    fseek(dataDictionary, currentEntityDirection, SEEK_SET);
    fread(currentEntity.name, DATA_BLOCK_SIZE, 1, dataDictionary);

    fseek(dataDictionary, sizeof(long) * 2, SEEK_CUR); 

    fread(&currentEntityDirection, sizeof(long), 1, dataDictionary);

    printf("\nEntity Name: %s\n", currentEntity.name);
  }
}*/

void showEntities(FILE* dataDictionary){
    long currentEntityDirection = -1;

    fseek(dataDictionary, MAIN_ENTITY_POINTER, SEEK_SET);
    fread(&currentEntityDirection, sizeof(currentEntityDirection), 1, dataDictionary);

    if (currentEntityDirection == EMPTY_POINTER) {
        printf("\nNo entities found.\n\n");
        return;
    }

    printf("\n--- Entities List ---\n");
    printf("Entity Name\tData Pointer\tAttributes Pointer\tNext Entity\t\n");
    while (currentEntityDirection != EMPTY_POINTER) {
        ENTITY currentEntity;

        fseek(dataDictionary, currentEntityDirection, SEEK_SET);
        fread(currentEntity.name, DATA_BLOCK_SIZE, 1, dataDictionary);
        fread(&currentEntity.dataPointer, sizeof(long), 1, dataDictionary);
        fread(&currentEntity.attributesPointer, sizeof(long), 1, dataDictionary);
        fread(&currentEntityDirection, sizeof(long), 1, dataDictionary);

        printf("%s\t%ld\t%ld\t%ld\t\n", currentEntity.name, currentEntity.dataPointer, currentEntity.attributesPointer, currentEntity.nextEntity);
        showAttributes(dataDictionary, currentEntity.attributesPointer);
    }
}

//Nuevas adiciones al programa

//Agregar metadatos
void captureMetadata(FILE* dataDictionary, ENTITY* currentEntity) {
    if (currentEntity->attributesPointer == EMPTY_POINTER) {
        printf("No attributes defined for this entity.\n");
        return;
    }

    ATTRIBUTE currentAttribute;
    long attributePointer = currentEntity->attributesPointer;
    char buffer[DATA_BLOCK_SIZE];
    long metadataPosition;

    // Mover al final del archivo para escribir los metadatos
    fseek(dataDictionary, 0, SEEK_END);
    metadataPosition = ftell(dataDictionary);

    // Capturar metadatos para cada atributo
    while (attributePointer != EMPTY_POINTER) {
        // Leer el atributo actual
        fseek(dataDictionary, attributePointer, SEEK_SET);
        fread(&currentAttribute, sizeof(ATTRIBUTE), 1, dataDictionary);

        // Capturar el dato para este atributo
        printf("Enter data for attribute '%s': ", currentAttribute.name);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Eliminar salto de línea

        // Validar tamaño del dato
        if (strlen(buffer) >= currentAttribute.size) {
            printf("Data too large for attribute '%s'. Max size: %ld.\n", currentAttribute.name, currentAttribute.size - 1);
            return;
        }

        // Escribir el dato al archivo
        fwrite(buffer, sizeof(char), currentAttribute.size, dataDictionary);

        // Mover al siguiente atributo
        attributePointer = currentAttribute.nextAttribute;
    }

    // Actualizar el puntero de datos de la entidad si es la primera vez
    if (currentEntity->dataPointer == EMPTY_POINTER) {
        currentEntity->dataPointer = metadataPosition;
        fseek(dataDictionary, -sizeof(ENTITY), SEEK_CUR);
        fwrite(currentEntity, sizeof(ENTITY), 1, dataDictionary);
    }

    printf("Metadata captured successfully.\n");
}

//Capturar metadatos
void captureMetaDataForEntity(FILE* dataDictionary) {
    char entityName[DATA_BLOCK_SIZE];

    showEntities(dataDictionary);

    printf("\nEnter the name of the entity to add attributes: ");
    fgets(entityName, sizeof(entityName), stdin);
    entityName[strcspn(entityName, "\n")] = '\0'; 

    ENTITY entity = searchEntityByName(dataDictionary, entityName);

    if (strlen(entity.name) > 0) {
        captureAttributes(dataDictionary, entity);
    } else {
        printf("\nCannot add attributes. Entity not found.\n");
    }
}

//Menu principal: Abrir o crear diccionario, salir del programa
void mainMenu() {
    FILE* dataDictionary = NULL;
    int option;
    char dictionaryName[DATA_BLOCK_SIZE];

    do {
        printf("\n--- Main Menu ---\n");
        printf("0) Exit\n");
        printf("1) Open Data Dictionary\n");
        printf("2) Create Data Dictionary\n");
        printf("Select an option: ");
        scanf("%d", &option);
        getchar(); 

        switch (option) {
            case 1:
                printf("\nEnter the name of the dictionary to open: ");
                fgets(dictionaryName, DATA_BLOCK_SIZE, stdin);
                dictionaryName[strcspn(dictionaryName, "\n")] = '\0';

                dataDictionary = fopen(dictionaryName, "r+");
                selectionEntitiesAttributes(dataDictionary);
                fclose(dataDictionary);
                break;

            case 2:
                printf("\nEnter the name for the new dictionary: ");
                fgets(dictionaryName, sizeof(dictionaryName), stdin);
                dictionaryName[strcspn(dictionaryName, "\n")] = '\0';

                dataDictionary = initializeDataDictionary(dictionaryName, "w+");
                selectionEntitiesAttributes(dataDictionary);
                fclose(dataDictionary);
                break;

            case 0:
                printf("\nExiting...\n");
                break;
                
            default:
                printf("\nInvalid option. Try again.\n");
        }
    } while (option != 0);
    
}

//Menu de entidades: Sub-menu del menu principal
void entityMenu(FILE* dataDictionary) {
    int option;

    do {
        printf("\n--- Entity Menu ---\n");
        printf("0) Back to Entityes and Attributes Menu\n");
        printf("1) Create Entity\n");
        printf("2) Delete Entity\n");
        printf("3) Modify Entity\n");
        printf("4) Print Entity List\n");
        printf("Select an option: ");
        scanf("%d", &option);
        getchar(); 

        switch (option) {
            case 1:
                captureEntities(dataDictionary);
                break;
            case 2:
                deleteEntity(dataDictionary);
                break;
            case 3:
                //modifyEntity(dataDictionary);
                break;
            case 4:
                showEntities(dataDictionary);
                break;
            case 0:
                printf("\nReturning to Entityes and Attribute Menu...\n");
                selectionEntitiesAttributes(dataDictionary);
                break;
            default:
                printf("\nInvalid option. Try again.\n");
        }
    } while (option != 0);
}

//Menu de atributos
void attributeMenu(FILE* dataDictionary) {
    int option;

    do {
        printf("\n--- Attribute Menu ---\n");
        printf("0) Back to Entityes and Attributes Menu\n");
        printf("1) Create Attribute\n");
        printf("2) Delete Attribute\n");
        printf("3) Modify Attribute\n");
        printf("4) Print Attribute List\n");
        printf("Select an option: ");
        scanf("%d", &option);
        getchar();

        switch (option) {
            case 1:
                captureAttributesForEntity(dataDictionary);
                break;
            case 2:
                deleteAttribute(dataDictionary);
                break;
            case 3:
                //modifyAttribute(dataDictionary);
                break;
            case 4:
                showEntitiesWithAttributes(dataDictionary);
                break;
            case 0:
                selectionEntitiesAttributes(dataDictionary);
                printf("\nReturning to Entityes and Attributes Menu...\n");
                break;
            default:
                printf("\nInvalid option. Try again.\n");
        }
    } while (option != 0);
}

void selectionEntitiesAttributes(FILE* dataDictionary) {
    int option; 

    do{ 
        printf("\n0)Back to main menu\n");
        printf("1) Entityes Menu\n");
        printf("2) Attributes Menu\n");
        printf("Select an option: ");
        scanf("%d", &option);
        getchar(); 
    
        switch (option) {
            case 1:
                entityMenu(dataDictionary);
                break;
    
            case 2: 
                attributeMenu(dataDictionary);
                break;
    
            case 0:
                printf("\nReturning to Main Menu...\n");
                mainMenu();
                break;

            default:
                break;
            }
         }
    while (option != 0);
}

//Borrar una entidad
void deleteEntity(FILE* dataDictionary) {
    char entityName[DATA_BLOCK_SIZE];

    showEntities(dataDictionary);

    printf("\nEnter the name of the entity to delete: ");
    fgets(entityName, sizeof(entityName), stdin);
    entityName[strcspn(entityName, "\n")] = '\0'; 

    ENTITY entity = searchEntityByName(dataDictionary, entityName);

    if (strcmp(entity.name, entityName) == 0) {
        long mainEntityPointer = EMPTY_POINTER;
        fseek(dataDictionary, MAIN_ENTITY_POINTER, SEEK_SET);
        fread(&mainEntityPointer, sizeof(long), 1, dataDictionary);

        ENTITY resultEntity = removeEntity(dataDictionary, mainEntityPointer, entityName);

        if (strlen(resultEntity.name) > 0) {
            printf("\nEntity '%s' deleted successfully.\n", entityName);
        } else {
            printf("\nFailed to delete entity '%s'.\n", entityName);
        }
    } else {
        printf("\nEntity '%s' not found. Cannot delete.\n", entityName);
    }
}

//Borrar un atributo de una entidad
void deleteAttribute(FILE* dataDictionary) {
