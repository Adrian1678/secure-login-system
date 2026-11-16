
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
 
// F                             Function that takes in user and pass info hashes it and saves it to a file
void create_account()
{
 
    // declaring variables
    char login[100];
    char pass[100];
    unsigned char buffer[32];
    FILE *data;
 
    // opeing a database file so i can save the hashed login information later on
    data = fopen("User_database.txt", "a");
 
    // taking users info
    printf("\n                                                  CREATE ACCOUNT\n");
    printf("Username:  ");
    scanf("%s", login);
 
    printf("\nPassword: ");
    scanf("%s", pass);
    printf("Hashed username: ");
    // hashing new account informatin
    SHA256(login, strlen(login), buffer); // username
    for (int i = 0; i < 32; i++)
    {
        printf("%02x", buffer[i]);
        fprintf(data, "%02x", buffer[i]); // saving it to user_database file
    }
    fprintf(data, "\n"); // saving to file
    printf("\n");
    printf("hashed password: ");
    SHA256(pass, strlen(pass), buffer); // password
    for (int i = 0; i < 32; i++)
    {
        printf("%02x", buffer[i]);
        fprintf(data, "%02x", buffer[i]); // saving it to user_database file
    }
    fprintf(data, "\n");
    // getting user to sign in
    printf("\n                                                     LOGIN SUCCESSFULL!!");
    printf("\n                                                  ENJOY 100+ MILLION SONGS!");
 
    // closing database file
    fclose(data);
}
 
//                          Function that takes a user login info--> hashes it--> compares and checks if it exists in the system[   if YES user is logged in, if NO user is now allowed in
void login_user(){
    //declaring variables
    char login[100];
    char pass[100];
    unsigned char buffer[100];
    char compare[100];
    char compare_pass[100];
    char question[100];
    char next_line[100];
    bool q=true;
    bool found=false;
// opening the database file
    FILE *data;
    data=fopen("User_database.txt", "r");
    //getting user to sign in
 
    printf("\n                                                  SIGN IN\n ");
    printf("Username: ");
    scanf("%s", login);
    printf("\n Password: ");
    scanf("%s", pass);
//converting the username to hash 
    SHA256(login, strlen(login), buffer);
    for (int i=0;i<32;i++) sprintf(compare + (i*2), "%02x", buffer[i]);
    compare[64] = '\0';
//password now 
    SHA256(pass, strlen(pass), buffer);
    for (int i=0;i<32;i++) sprintf(compare_pass + (i*2), "%02x", buffer[i]);
    compare_pass[64] = '\0';
 
    char line[100];//comparing the hash to the database hash
    while (fgets(line, sizeof(line), data) != NULL) {
        //removing the last space on everyline
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, compare) == 0) {
            //now for the password
            if (fgets(next_line, sizeof(next_line), data) == NULL) break;
            next_line[strcspn(next_line, "\n")] = 0;
            //if login successful/ wrong
            if (strcmp(next_line, compare_pass) == 0) {
                printf("\nLOGIN SUCCESSFUL! ENJOY 100+ MILLION SONGS!\n");
                found=true;
            } else {
                printf("\nWrong password!\n");
                found=true;
            }
            break;
        }
    }
    if (!found) {//if the account is not found
        printf("\nAccount not found!\n");
        printf("\n     ACCOUNT DOES NOT EXIST or LOGIN INFO INCORRECT | TRY AGAIN(t) | CREATE ACCOUNT(c) | EXIT(e)\n");
        printf(": ");
 
        // incase the user makes a mistake
        while (q)
        {
            scanf("%s", question);
            if (strcmp(question, "c") == 0)
            { // if they want to create a new account
                create_account();
                q = false;
            }
            else if (strcmp(question, "t") == 0)
            { // if they want to try again
                login_user();
                q = false;
            }
            else if (strcmp(question, "e") == 0)
            { // if they want to exit
                q = false;
            }
            else
            {
                printf("incorrect value entered | try again!");
            }
    }
    
    }
    //closing the file
    fclose(data);
}
/*implementing AES so i can encryt the database in the memory 
so an attacker can't just wait untill its opened to attack*/
 
void load_key(unsigned char *key){
    //declaring variables  
    FILE *key_file;
 
    //opeing a AES file containing the key
    key_file=fopen("not_the_key.bin", "rb");
    //got command line to generate a 32 bit code with openssl rand not_the_key.bin 32
    //getting the code
    fread(key, 1, 32, key_file);
    //removing the last space in the line
          
    //closing the file
    fclose(key_file);
 
}
 
void encrypt_dabase(unsigned char *key){
    //declaring variables
    FILE *database;
    long size;
    unsigned char *buffer;
    unsigned char iv[16];
    unsigned char *encrypted;
    FILE *User_database_enc;
    AES_KEY aes_key;
    database=fopen("User_database.txt", "rb");
    if (database ==NULL){
        printf("\n file is empty");
        return;
    }
    
    //going to the end 
    fseek(database, 0, SEEK_END);
    //getting the size
    size =ftell(database);
    //going to the begining
    fseek(database,0,SEEK_SET);
    buffer= malloc(size);
    //reading the file into buffer
    fread(buffer, 1, size, database);
 
    RAND_bytes(iv, 16);
    AES_set_encrypt_key(key, 256, &aes_key);
    //alocating the necessary memory to encrypted
    encrypted= malloc(((size / 16)+1)*16);
 
    //encrypting the data
    AES_cbc_encrypt(buffer, encrypted, size, &aes_key, iv, AES_ENCRYPT);
 
    //creating the encrypted file
    User_database_enc=fopen("Encrypted_User_database.enc", "wb");
 
    fwrite( iv,1, 16, User_database_enc );//plaicing the iV into it
    fwrite(encrypted,1, ((size / 16)+1)*16,User_database_enc);//placing the encrypted data
 
    //preventing memory leaks
    free(buffer);
    free(encrypted);
 
    //closing file
    fclose(database);
    fclose(User_database_enc);
 
    
}
 
//decrypting the file
void decryp_database(unsigned char *key){
    //declaring variables
    long size;
    unsigned char *buffer;
    unsigned char iv[16];
    unsigned char *decrypted;
    AES_KEY aes_key;
    FILE* file;
 
    FILE *encrypted_database;
    //reading the first 16bytes
    encrypted_database= fopen("Encrypted_User_database.enc", "rb");//opeingt the encrypted database
    fread(iv,1, 16, encrypted_database);  //reading the first 16 bytes
 
    //getting the decrypting key
    AES_set_decrypt_key(key, 256, &aes_key);
 
    //reading the size of the data
    fseek(encrypted_database,0, SEEK_END);
    size= ftell(encrypted_database);
    //getting the the total size for a memory buffer
    decrypted = malloc(size);
    //going to the begining
    fseek(encrypted_database,16, SEEK_SET);
    buffer=malloc(size-16);//dynamically sizing
    fread(buffer,1, size-16,encrypted_database );
 
    //decrypting for real
    AES_cbc_encrypt(buffer, decrypted , size-16 , &aes_key,iv,AES_DECRYPT);
 
    //opeing the user_database
    file=fopen("User_database.txt", "wb");
 
    fwrite(decrypted,1, size-16,file);
 
 
    
    //preventing memory leaks
    free(buffer);
    free(decrypted);
 
    
    //closing the file
    fclose(encrypted_database);
    fclose(file);
 
 
 
}   
 
 
 
 
int main()
    {
        
        // declaring variables
        char q_one[50];
        unsigned char key[32];
        bool p = true;
 
        //load_key(key);
 
        //decryp_database(key);
        printf("                                                   WELCOME TO SPOTIFY!!\n");
        sleep(1);
        printf("                                           we got some great jams waiting for you!!\n");
        sleep(1);
        // asking the user if they want to create a new account or sign in
        printf("            CREATE ACCOUNT(c) | SIGN IN(s): ");
 
        // making sure no mistakes are made
        while (p)
        {
            scanf("%s", q_one);
            if (strcmp(q_one, "c") == 0)
            {
                create_account();
                p = false;
            }
            else if (strcmp(q_one, "s") == 0)
            {
                login_user();
                p = false;
            }
            else
            {
                printf("\nincorrect value entered | try again!");
                p = true;
            }
        }
        //encrypt_dabase(key);
 
        return 0;
    }