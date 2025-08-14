#include <stdio.h>
#include <stdbool.h>

int main() {
    double num1;
    double num2;
    double result;
    char operator;

    printf("Enter the operator: ");
    scanf("%c", &operator);
    
    printf("Enter the first number: ");
    scanf("%lf", &num1);
    printf("Enter the second number: ");
    scanf("%lf", &num2);



    switch (operator) {
        case '+':
            result = num1 + num2;
            printf("Result: %0.2lf\n", result);
            break;
        case '-':
            result = num1 - num2;
            printf("Result: %0.2lf\n", result);
            break;
        case '*':
            result = num1 * num2;
            printf("Result: %0.2lf\n", result);
            break;
        case '/':
            result = num1 / num2;
            printf("Result: %0.2lf\n", result);
            break;
        default:
            printf("Invalid operator\n");
    }

}