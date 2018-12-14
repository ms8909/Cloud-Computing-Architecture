    /* read the content of the files. */
    int mkf, codef, i, j;
    char buffer_m [BUFM];
    char buffer_c [BUFC];

    mkf = open(argv[2], O_RDWR, 0600);
    if (mkf == -1) {
        perror("Error when opening the make file.");
        exit(1);}
    
    codef = open(argv[3], O_RDWR, 0600);
    if (codef == -1) {
        perror("Error when opening the code file.");
        exit(1);}

    i = read(mkf, buffer_m, BUFM);
    printf("%s\n", buffer_m);

    i = read(codef, buffer_c, BUFC);
    printf("%s\n", buffer_c);
