
int a = 2;
int b = 3;
int c = 0;
int i = 0;

print(a);
print(b);
print(c);

while (i <= 5) {
    c = c + i;
    print(c);
    i = i + 1;
}


for (int j = 0; j < 3; j = j + 1) {
    c = c + j;
    print(c);
}

int e = c - a;
print(e);

int arr[5];
arr[0] = 10;
arr[1] = 20;
print(arr[0]);

if (e > 0) {
    e = e + 10;
} else {
    e = e - 10;
}


return e;