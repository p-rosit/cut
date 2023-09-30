#! /bin/bash

test_files_chosen=false
continue_on_error=false
compiler_chosen=false
flags_chosen=false

while getopts ':t:c:aeh' OPTION; do
    case "$OPTION" in
        t)
            test_files=$OPTARG
            test_files_chosen=true
            ;;
        c)
            compiler=$OPTARG
            compiler_chosen=true
            ;;
        a)
            flags=$OPTARG
            flags_chosen=true
            ;;
        e)
            continue_on_error=true
            ;;
        h)
            echo "Usage: unit_testing [-e] [-c compiler] [-a compiler_args]"
            echo ""
            echo "-e        Flag for specifying continue on error. If present"
            echo "          unittesting of a file is not stopped when a test fails."
            echo ""
            echo "-c        The compiler to use, the default is gcc."
            echo ""
            echo "-a        Extra arguments for the compiler"
            exit
            ;;
    esac
done

if [ "$compiler_chosen" = false ] ; then
    compiler=gcc
fi

if [ "$flags_chosen" = false ] ; then
    flags="-Wall -Wpedantic"
fi

if [ "$continue_on_error" = true ] ; then
    flags="$flags -DUNIT_TEST_CONTINUE_ON_FAIL"
fi

if [ "$test_files_chosen" = false ] ; then
    test_directories=$(find * -type d -name 'unit_tests*')
    test_files=$(find * -type f -name 'unit_tests_*.c')
fi

echo "-------------------------------------------------------------------------------"
echo "|                              Running unittests                              |"
echo "-------------------------------------------------------------------------------"

run_test () {
    $compiler $flags $1
    if [ $? -eq 0 ] ; then
        ./a.out
        rm ./a.out
    fi
}

if [ "$test_files_chosen" = false ] ; then
    for d in $test_directories
    do
        echo "Running unit tests under: $(tput bold)$d$(tput sgr0)"
        for f in "$d"/*
        do
            echo "$(tput bold)$f$(tput sgr0)"
            run_test $f
        done
        echo ""
    done

    echo "Running scattered unit tests:"
    for f in $test_files
    do
        file_exists=false
        for d in $test_directories
        do
            temp=$(find $d -wholename $f)
            if [ -n "$temp" ] ; then
                file_exists=true
            fi
        done

        if [ "$file_exists" = false ] ; then
            echo "$(tput bold)$f$(tput sgr0)"
            run_test $f
        fi
    done

else
    # TODO
    echo "$test_files"
    for f in $test_files
    do
        echo "$(tput bold)$f$(tput sgr0)"
    done
fi

