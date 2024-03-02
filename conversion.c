// this header is to convert hex to ascii and vice versa
#include <stdio.h>
#include <ctype.h>


char hex_to_ascii()
{
    return 0;
}


char* binary_conversion(char* hex_value)        // doesnt work ignore this 
{
    const char input[] = "A"; // the value to be converted
    char res[9]; // the length of the output string has to be n+1 where n is the number of binary digits to show, in this case 8
  
    int t = 128; // set this to s^(n-1) where n is the number of binary digits to show, in this case 8
    int v = strtol(input, 0, 16); // convert the hex value to a number

    while(t) // loop till we're done
    {
        strcat(res, t < v ? "1" : "0");   // contatenates strings  
        if(t < v)
            v -= t;
        
        t /= 2;
    }

    static char final[8];
    
    for (int i = 2; i <= strlen(res); i++)      // this function removes the 
    {
        final[i-2] = res[i];
    }

    return final;
}

int flip_bits(char bit_value)       // flips the bits of the 2nd nibble
{
    int final_value, hex_value;     // final_value = flipped value, hex_value = converts hex char into actual hex

    if (bit_value >= '0' && bit_value <= '9') // if hex value is between 0 and 9
    {
        hex_value = bit_value - '0';          // substracts 0 from the character to give the hex value
    }
    
    else                    // if hex value is between A and F
    {
        hex_value = bit_value - 'A' + 10;    // substracts A, which gives value in hex and then adds 10 which gives values greater than 9
    }

    // (hex_value & 0xf0) ensures that the first nibble in no way altered
    // (~(hex_value & 0x0F) & 0x0F)) flips the bits of the second nibble
    // + performs bitwise addition here

    /*
        the reason for ANDing with 0xf0:
        - 0xF0 essentially means 11110000 and 0x0F means 00001111
        - Anding with F0 means that the 2nd nibble of the value becomes 0000 and the other half remains unchanged
        - Anding with 0F means that the 1st nibble of the value becomes 0000 and the other half remains unchanged
        - This lets for a proper binary operation
        
        For example, Say hex value is 10100011, ANDing with F0, we get 10100000, ANDing with 0F, we get 00000011 
        Negate this value and we get 11111100, ANDing this value, we get 00001100.
        Add 10100000 and 00001100, we get 10101100.

    */ 
    final_value = (int) ((hex_value & 0XF0) + (~(hex_value & 0x0F) & 0x0F));

    return final_value;
}

int count_return_factors(int number)
{
    int element_count = 0;


    for (int i = 1; i < number; i++ )
    {
        if (number % i == 0)
        {
            element_count++;
        }
    }

    return element_count;
}


void return_factors(int number, int factors_array[])
{
    int element_count = 0;

    for (int i = 2; i < number; i++ )
    {
        if (number % i == 0)
        {
            factors_array[element_count] = i;
            element_count++;
        }
    }
}

int returning_wrapped_positions(int factor) 
{
    int wrapped_position;
    wrapped_position = factor % 16;

    return wrapped_position;
}

int poly_function(int x)
{
    int y;

    y = 3 * ( x ^ (3) ) + (( x ^ ( 1 / 2 ) ) * 500) + 101;

    if (count_return_factors(y) == 0)
    {
        printf("y is a prime number, try again.");
        return 0;
    }
    return y;
}

char* dec_input_to_hex(unsigned long A)
{
    static char a_in_hex[10];       // this makes the value of a_in_hex exist for the duration of the program
    // the reason for this acting up was actually just forgetting to allocate memory for the null terminator
    unsigned long num = A;
    

    sprintf(a_in_hex, "%X", A);

    return a_in_hex;
    
}

int hex_into_dec(char hex_value)        // using a char for taking 16 bit value does work but gives an overflow, which for some reason works regardless
// trying to fix the overflow was more tedious so I left it as is
{
    int dec_value;
    char hex_value_[10];

    sprintf(hex_value_, "%X", hex_value);
    

    if (hex_value_[0] >= '0' && hex_value_[0] <= '9')
    {
        dec_value = hex_value_[0] - 48;
    }

    else if (hex_value_[0] >= 'A' && hex_value_[0] <= 'F')
    {
        dec_value = hex_value_[0] - 65 + 10;
    }


    return dec_value;
}

char* replacing_array_positions(char A[10], char B[10], int position_value[], int number_of_values)
{

        char placeholder[1];
        int flipped_bit, placeholder_final;

        for (int i = 0; i < number_of_values; i++)
        {
            if (position_value[i] < 8)
            {
                flipped_bit = flip_bits(A[position_value[i]]);
                sprintf(placeholder, "%X", flipped_bit);
                placeholder_final = placeholder[0];
                A[position_value[i]] = placeholder_final;
            }

            else if (position_value[i] >= 8 && position_value[i] < 16)
            {
                flipped_bit = flip_bits(B[position_value[i] - 8]);
                sprintf(placeholder, "%X", flipped_bit);
                placeholder_final = placeholder[0];
                B[position_value[i] - 8] = placeholder_final;
            }

        }

        printf("\nFinal value of A: %s", A);
        printf("\nFinal value of B: %s", B);



    return 0;
}