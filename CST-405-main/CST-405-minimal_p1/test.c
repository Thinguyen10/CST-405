
int a = 2;
int b = 3;
int c = 0;
int i = 0;

print(a);
print(b);
print(c);

while (i <= 5) {
    print(i);
    c = c + i;
    print(c);
    i = i + 1;
}

print(c);

int d = a * b;
print(d);

int e = c - d;
print(e);


if (e > 0) {
    e = e + 10;
} else {
    e = e - 10;
}


return e;
