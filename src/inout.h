/* This file is intended to be used as a generic wrapper to run codes. It does
 * the following:
 *
 *(a) Takes an input file <file> , or input file directory <path>/, from the
 *    command line. If an input file is not given, then it will look for
 *    ./user.i <path>/user.i. If these files are not found, then it exits
 *    on error.
 *
 *(b) It reads the input file into the object "user", at which point
 *    all input file parameters are available as "user.value".
 *
 *(c) Generates the output directory <user.outdir> (the user input files must
 *    contain the keyword outdir).
 *
 *(d) Echoes <input_file> and <input_file>.echo to the output directory.
*/

#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "InputReader_c.h"
#include <TString.h>

TextFileInputs read_user_input_textfile(const char* fin){
    TString fin_name = fin;
    struct stat fstat;
    struct stat fstat2;

 /* (a) */
    // get the input file/directory name */
    /* if (argc == 1) { */
    /*     if (access( "./user.i", F_OK ) == -1 ) { */
    /*         std::cout << "Exit on error. This program requires either:\n" */
    /*                   << "   (1) A at least one input parameter which names the input file\n" */
    /*                   << "       or input file directory.\n" */
    /*                   << "   (2) No input parameters. Defaults to user.i.\n" */
    /*                   << std::flush; */
    /*         exit(1); */
    /*     } */
    /*     fin_name = "./user.i"; */
    /* } else { */
        //fin_name = argv[1];
        if (!fin_name.EndsWith("/") && access(fin_name.Data(), F_OK) == -1, F_OK){
            std::cout << "Exit on error. The input file cannot be accessed.\n" << std::flush;
            exit(1);
        }
    /* } */ 

    // check for file if a directory name was given.
	if (fin_name.EndsWith("/")) {
        if (access(fin_name.Data(), F_OK) == -1) {
            std::cout << "Exit on error. Input directory cannot be accessed.\n"
                      << std::flush;
            exit(1);
        }
        bool found_file = false;
		TString temp = fin_name;
		temp.Append("user.i");
		if (access(temp.Data(), F_OK) == -1) {
            std::cout << "Exit on error. The input directory does not contain\n"
                      << "the file user.i.\n" << std::flush;
            exit(1);
        }
    }

    //check if the input file is really a file or a directory
    if ( stat(fin_name.Data(),&fstat) == 0 ) {
        if ( fstat.st_mode & S_IFDIR ) {
            std::cout << "Exit on error, the input file is a directory.\n"
                      << std::flush;
            exit(1);
        } else if (!(  fstat.st_mode & S_IFREG )) {
            std::cout << "Exit on error, the input file is not a regular file.\n"
                      << std::flush;
            exit(1);
        }
    } else {
        std::cout << "Exit on error, the input file was not found.\n" << std::flush;
        exit(1);
    }

 /* (b) */
    // read the input file
    InputReader inp_reader(fin_name.Data());
    TextFileInputs user(inp_reader);
    inp_reader.close_files();

 /* (c) */
    // if output directory doesn't exist, make it
    const char* odir = user.outdir.data();
    if (stat(odir, &fstat) == -1) {
        mkdir (odir, 0700);
    } else if (fstat.st_mode & S_IFREG) {
        std::cout << "Exit on error. The output directory name " << odir
                  << " is already the name of a file\n" << std::flush;
        exit(1);
    }

/* (d) */
    // copy both the input file, and the input file echo, to the output directory
    TString tstr1;
    TString tstr2;
    tstr1.Form("%s.echo",fin_name.Data());
    tstr2.Form("%s/%s.echo",odir,fin_name.Data());

    stat(tstr1.Data(),&fstat);
    stat(tstr2.Data(),&fstat2);
    if (fstat.st_dev == fstat2.st_dev && fstat.st_ino == fstat2.st_ino){
        // do nothing. They are the same files and should not be copied
    } else {
        std::ifstream srce(tstr1.Data(),std::ios::binary);
        std::ofstream dest(tstr2.Data(),std::ios::binary);
        dest << srce.rdbuf();

        tstr1.Form("%s",fin_name.Data());
        tstr2.Form("%s/%s",odir,fin_name.Data());
        {
            std::ifstream srce(tstr1.Data(),std::ios::binary);
            std::ofstream dest(tstr2.Data(),std::ios::binary);
            dest << srce.rdbuf();
        }
    }
    return user;

  //end of stated goals
    //std::cout << user.outdir.data() << std::endl;
}
