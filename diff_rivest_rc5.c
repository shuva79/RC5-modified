#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "conversion.c"

typedef unsigned long int WORD; /* Should be 32-bit = 4 bytes        */
#define w        32             /* word size in bits                 */
#define r        20             /* number of rounds                  */  
#define b        16             /* number of bytes in key            */
#define c         4             /* number  words in key = ceil(8*b/w)*/
#define t        26             /* size of table S = 2*(r+1) words   */
WORD S[t];                      /* expanded key table                */
WORD P = 0xb7e15163, Q = 0x9e3779b9;  /* magic constants             */
/* Rotation operators. x must be unsigned, to get logical right shift*/
#define ROTL(x,y) (((x)<<(y&(w-1))) | ((x)>>(w-(y&(w-1)))))
#define ROTR(x,y) (((x)>>(y&(w-1))) | ((x)<<(w-(y&(w-1)))))

void RC5_ENCRYPT(WORD *pt, WORD *ct, unsigned char* key) /* 2 WORD input pt/output ct    */
{ 
  WORD i, A = pt[0] + S[0], B = pt[1] + S[1];   // plaintext for encryption in rc5
  char hex_convert_A[10], hex_convert_B[10];    // this is for holding the hex value of A and B


  printf("\nFor encryption, we perform the operation ((A XOR B) << B) + S[2*i] on A and ((B XOR A) << A)+ S[2*i+1] on B for a total of 20 rounds or 40-half rounds.\n");
  for (i = 1; i <= r; i++)      // this encrypts the plaintext for a total of 40 rounds
  { 
      A = ROTL(A ^ B, B) + S[2*i]; 
      printf("\nA at round %d is %X. Initially, A XOR'd with B, gives %X. This is then left shifted to give %X. This is then added with the key table S[%d] to give %X.\n", i, A, A^B, ROTL(A ^ B, B), 2*i, A);
      B = ROTL(B ^ A, A) + S[2*i+1]; 
      printf("\nB at round %d is %X. Initially, B XOR'd with A, gives %X. This is then left shifted to give %X. This is then added with the key table S[%d] to give %X.\n", i, B, B^A, ROTL(B ^ A, A), 2*i+1, B);
  }

  // this is for testing for key
//   printf("\nKey visible while encrypting\n");
//   for (WORD k=0;k<b;k++) printf("%.2X ", key[k]);  
  
  // this is for the modification part
  int key_avg = ( hex_into_dec(key[1]) + hex_into_dec(key[15]) ) / 2; //this takes two values from the key and then passes it into poly_function
  int ppap = poly_function(key_avg);        // this returns a sufficiently large value for further modification
  int count_total_factors = count_return_factors(ppap);                         // this counts the number of factors a number may produce

  printf("\nThe average values of %d and %d is taken to give %d. %d when passed through the polynomial function, gives us %d.\n", hex_into_dec(key[1]), hex_into_dec(key[15]), key_avg, key_avg, ppap);
  // this creates two arrays of dynamic size, it reserves the size of the array beforehand
  // I havent freed the allocate memory so this might be a little unoptimised. I cant take any risk by trying to free the memory
  int *factors_array =  malloc(count_total_factors * sizeof(int));            // this array stores the factors of a given number
  int *wrapped_factors_array = malloc(count_total_factors * sizeof(int));     // this array stores the values of factor mod 16

  //printf("\n%d\n", ppap);           // this is the sufficiently large value
  return_factors(ppap, factors_array);  // this creates the array 

  // this is to check the factors of any given number
  printf("\nThe factors from the resulting value of the function %d are: ", ppap);

  // this shows the actual number of arrays
  for (int j = 0; j < count_total_factors - 1; j++)
  {
      printf("%d\t", factors_array[j]);         // shows the factors from ppap
      wrapped_factors_array[j] = returning_wrapped_positions(factors_array[j]); // wrapped values just mean the value % 16 
  }
  printf("\n");

  // A and B's values are in unsigned long int form. We have to convert it into hex to display it as ciphertext
  strcpy(hex_convert_A, dec_input_to_hex(A));       
  strcpy(hex_convert_B, dec_input_to_hex(B));
  printf("\nThe first part of encryption yields the following values for registers A and B: %X and %X \n", A, B);
 

  printf("\nUpon performing a modulus 16 operation with each of the elements of the array, we get the following values: ");
  for (int i = 0; i < count_total_factors - 1; i++)
  {
      printf("%d\t", wrapped_factors_array[i]);       // this prints the array % 16 values
  }

  // this replaces the RC5 ciphertext with the bitflipped values at the respective positions
  replacing_array_positions(hex_convert_A, hex_convert_B, wrapped_factors_array, count_total_factors);
  

  // this converts the final modified ciphertext which is a string into an unsigned long int
  WORD final_ciphertext_A = strtoul(hex_convert_A, '\0', 16);
  WORD final_ciphertext_B = strtoul(hex_convert_B, '\0', 16);
  printf("\nThe final converted ciphertext (first word) is: %X\n", final_ciphertext_A);
  printf("\nThe final converted ciphertext (second word) is: %X\n", final_ciphertext_B);
  
  // this assigns the modified ciphertext as the final resultant ciphertext
  ct[0] = final_ciphertext_A; 
  ct[1] = final_ciphertext_B;  


} 

void RC5_DECRYPT(WORD *ct, WORD *pt, unsigned char* key) /* 2 WORD input ct/output pt    */
{ 
  WORD i, B = ct[1], A = ct[0];
  char hex_convert_A[10], hex_convert_B[10];    // this is for holding the hex value of A and B
  
  printf("\nFor decryption, the process is in inverse order. First, we use our modified algorithm to rearrange the replaced positions and then standard decryption is applied.\n");
  int key_avg = ( hex_into_dec(key[1]) + hex_into_dec(key[15]) ) / 2; //this takes two values from the key and then passes it into poly_function
  int ppap = poly_function(key_avg);
  int count_total_factors = count_return_factors(ppap);                         // this counts the number of factors a number may produce
  int *factors_array =  malloc(count_total_factors * sizeof(int));            // this array stores the factors of a given number
  int *wrapped_factors_array = malloc(count_total_factors * sizeof(int));     // this array stores the values of factor mod 16

  //printf("\n%d\n", ppap);
  printf("\nThe average values of %d and %d is taken to give %d. %d when passed through the polynomial function, gives us %d.\n", hex_into_dec(key[1]), hex_into_dec(key[15]), key_avg, key_avg, ppap);
  return_factors(ppap, factors_array);

  // this is to check the factors of any given number
  printf("\nThe factors from the resulting value of the function %d are: ", ppap);

  // this shows the actual number of arrays
  for (int j = 0; j < count_total_factors - 1; j++)
  {
      printf("%d\t", factors_array[j]);
      wrapped_factors_array[j] = returning_wrapped_positions(factors_array[j]);
  }
  printf("\n");

  strcpy(hex_convert_A, dec_input_to_hex(A));
  strcpy(hex_convert_B, dec_input_to_hex(B));
 
  printf("The first part of encryption yields the following values for registers A and B: %X and %X \n", A, B);
  printf("Upon performing a modulus 16 operation with each of the elements of the array, we get the following values: ");

  for (int i = 0; i < count_total_factors - 1; i++)
  {
      printf("%d\t", wrapped_factors_array[i]);
  }

  replacing_array_positions(hex_convert_A, hex_convert_B, wrapped_factors_array, count_total_factors);
  

  WORD final_plaintext_A = strtoul(hex_convert_A, '\0', 16);
  WORD final_plaintext_B = strtoul(hex_convert_B, '\0', 16);
  
  printf("\nFor decryption, we perform the operation A = ((A - S[2*i] >> B) XOR B) on A and ((B - S[2*i+1] >> A) XOR A) on B for a total of 20 rounds or 40-half rounds.\n");
  for (i = r; i > 0; i--) 
  { 
      final_plaintext_B = ROTR(final_plaintext_B-S[2*i+1], final_plaintext_A)^ final_plaintext_A; 
      
      final_plaintext_A = ROTR(final_plaintext_A-S[2*i], final_plaintext_B)^ final_plaintext_B; 
      
      printf("\nA at round %d is %X. Initially, S[%d] whose value is %u is subtracted from A, giving %X. This is then right shifted by B of value %X to give %X. This is then XOR'd with B again to give %X.\n", r-i, final_plaintext_A, 2*i, S[2*i], final_plaintext_A-S[2*i], ROTR(final_plaintext_A-S[2*i], final_plaintext_B), ROTR(final_plaintext_A-S[2*i], final_plaintext_B)^ final_plaintext_B);
      printf("\nB at round %d is %X. Initially, S[%d] whose value is %u is subtracted from B, giving %X. This is then right shifted by A of value %X to give %X. This is then XOR'd with A again to give %X.\n", r-i, final_plaintext_B, 2*i, S[2*i+1], final_plaintext_B-S[2*i+1], ROTR(final_plaintext_B-S[2*i+1], final_plaintext_A), ROTR(final_plaintext_B-S[2*i+1], final_plaintext_A)^ final_plaintext_A);
  }
    
  printf("\nA at round %d is %X. Initially, S[%d] whose value is %u is subtracted from A, giving %X. Since only addition takes place for the first round, we do not need to right shift it.\n", r-i, final_plaintext_A, 2*i, S[2*i], final_plaintext_A-S[0]);
  printf("\nB at round %d is %X. Initially, S[%d] whose value is %u is subtracted from B, giving %X. Since only addition takes place for the first round, we do not need to right shift it.\n", r-i, final_plaintext_B, 2*i+1, S[2*i+1], final_plaintext_B-S[1]);
  printf("\nConverted plaintext (first word): %X\n", final_plaintext_A - S[0]);
  printf("\nConverted plaintext (second word): %X\n", final_plaintext_B - S[1]);
  
  pt[1] = final_plaintext_B-S[1]; 
  pt[0] = final_plaintext_A-S[0];  
} 

void RC5_SETUP(unsigned char *K) /* secret input key K[0...b-1]      */
{  
    WORD i, j, k, u = w / 8, A, B, L[c]; 
    
    printf("\nThe key is first expanded into a dynamic S table. There are a total of 16 subkeys.\n");
    printf("\nKey expansion: ");
    printf("\nThe secret key is now expanded into a total of 16 subkeys, the first step being conversion of keys from bytes to words.\n");
    /* Initialize L, then S, then mix key into S */
    printf("Converting keys from bytes to words:\n");
    for (i = b - 1, L[c-1] = 0; i != -1; i--)  
    {
        L[i/u] = ( L[i/u] << 8) + K[i];
        printf("\nSubkey at round %d is %u\n", b - i, L[i/u]);
    } 

    
    printf("\nInitializing the array S\n");
    for (S[0] = P, i = 1; i < t; i++) 
    {
      S[i] = S[i-1] + Q;
      printf("The value of S table at position %d is %u\n", i, S[i]);
    }  

    for (A = B = i = j = k = 0; k <3 * t; k++, i= (i+1) % t, j = (j+1) % c)   
    { 
        A = S[i] = ROTL(S[i] + (A+B), 3);  
        B = L[j] = ROTL(L[j] + (A+B), (A+B)); 
    } 
    printf("\nFinally, we mix the keys of the arrays S and L, the final values of registers A and B are %u and %u respectively.\n", A, B);
} 

int main(int argc, char *argv[])
{ 
  WORD i, j, pt1[2], pt2[2], ct[2] = {0,0};
  unsigned char key[b];
  unsigned char val1[100];
  unsigned char val2[100];
  time_t t0, t1;

  if (sizeof(WORD)!=4) 
    printf("RC5-32/12/16\n");

    pt1[0]=ct[0]; pt1[1]=ct[1]; 

    for (j=0;j<b;j++) key[j] = 1;
    char *key1 = "00000000000000000000000000";
	 
    strcpy(val1, "00000000");
	  strcpy(val2 , "00000000");


    if (argc>1)                           key1=argv[1];
	 
    if (argc>2) strcpy(val1, argv[2]);    printf("\nFirst word register is: %s\n", val1);
	  
    if (argc>3) strcpy(val2, argv[3]);    printf("\nSecond word register is: %s\n", val2);

	  pt1[0] = (WORD)strtoul(val1, NULL, 16);   // unsigned long so as to avoid overflow and accomodate higher ranges
	  pt1[1] = (WORD)strtoul(val2, NULL, 16);

	  unsigned int bytearray[12];
	  int str_len = strlen(key1);

	  for (i = 0; i < (b); i++)
      {
		  sscanf(key1 + 2 * i, "%02x", &key[i]);
	  }

    /* Print out results */
    printf("\nThe key given is: ");
	for (j = 0; j < b; j++) printf("%.2X ", key[j]);
    printf("\n");

    /* Setup, encrypt, and decrypt */
    RC5_SETUP(key);  


      if (argc>4) 
      {
        if (strcmp(argv[4],"d") == 0)
        {
            RC5_DECRYPT(pt1, pt2, key); 
            printf("\nThus, the final decrypted plaintext is as follows:\n");
            printf("\nCiphertext %.8lX %.8lX  --->  Plaintext %.8lX %.8lX  \n", pt1[0], pt1[1], pt2[0], pt2[1]); 
            return 0;
        
            /* checking for decryption failure */
            if (pt1[0]!=pt2[0] || pt1[1]!=pt2[1]) 
            printf("\nDecryption Error!");
            
        }

        else if (strcmp(argv[4], "e") == 0)
        {
            RC5_ENCRYPT(pt1, ct, key);  
            printf("\nThus, the final encrypted ciphertext is as follows:\n");
            printf("\nPlaintext %.8lX %.8lX  --->  Ciphertext %.8lX %.8lX  \n", pt1[0], pt1[1], ct[0], ct[1]);
        }

        //RC5_DECRYPT(ct,pt2, key);
      }   
      
      return 0;
}

