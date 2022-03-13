void profile() {
    blocks_table table =  create_table(10);

//    char* small_paths = "../test_files/small1.txt ../test_files/small2.txt ../test_files/small3.txt";
//    char* medium_paths = "../test_files/medium1.txt ../test_files/medium2.txt ../test_files/medium3.txt";
//    char* large_paths = "../test_files/large1.txt ../test_files/large2.txt ../test_files/large3.txt ";

    char* small_paths = "./test_files/small1.txt ./test_files/small2.txt ./test_files/small3.txt";
    char* medium_paths = "./test_files/medium1.txt ./test_files/medium2.txt ./test_files/medium3.txt";
    char* large_paths = "./test_files/large1.txt ./test_files/large2.txt ./test_files/large3.txt ";

    //Structs for saving timestamps
    struct timespec mt1, mt2;
    //Variable for time delta calculating
    long int tt;
    //Get current time
    clock_gettime (CLOCK_REALTIME, &mt1);

    FILE* file_s = wc_files(small_paths);

    //Get current time again
    clock_gettime (CLOCK_REALTIME, &mt2);
    //Calculate the delta between two timestamps
    tt=1000000000*(mt2.tv_sec - mt1.tv_sec )+(mt2.tv_nsec - mt1.tv_nsec);
    //Print the delta
    printf ("WC small test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

    clock_gettime (CLOCK_REALTIME, &mt1);
    FILE* file_m = wc_files(medium_paths);
    clock_gettime (CLOCK_REALTIME, &mt2);
    tt=1000000000*(mt2.tv_nsec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
    printf ("WC medium test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

    clock_gettime (CLOCK_REALTIME, &mt1);
    FILE* file_l= wc_files(large_paths);
    clock_gettime (CLOCK_REALTIME, &mt2);
    tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
    printf ("WC large test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

    //
    // saving test_files
    //
    clock_gettime (CLOCK_REALTIME, &mt1);
    save_block(file_s, &table);
    clock_gettime (CLOCK_REALTIME, &mt2);
    tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
    printf ("SAVING small test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

    clock_gettime (CLOCK_REALTIME, &mt1);
    save_block(file_m, &table);
    clock_gettime (CLOCK_REALTIME, &mt2);
    tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
    printf ("SAVING medium test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

    clock_gettime (CLOCK_REALTIME, &mt1);
    save_block(file_l, &table);
    clock_gettime (CLOCK_REALTIME, &mt2);
    tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
    printf ("SAVING large test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

    //
    // removing blocks from memory
    //
    clock_gettime (CLOCK_REALTIME, &mt1);
    remove_block(&table, 0);
    clock_gettime (CLOCK_REALTIME, &mt2);
    tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
    printf ("DELETING small test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

    clock_gettime (CLOCK_REALTIME, &mt1);
    remove_block(&table, 0);
    clock_gettime (CLOCK_REALTIME, &mt2);
    tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
    printf ("DELETING medium test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

    clock_gettime (CLOCK_REALTIME, &mt1);
    remove_block(&table, 0);
    clock_gettime (CLOCK_REALTIME, &mt2);
    tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
    printf ("DELETING large test_files: %ld nsec or %ld ms\n", tt, tt/1000000);

}