#! /bin/bash

while getopts 'f:d:c:a:O:e' OPTION; do
    case "$OPTION" in
        f)
            test_file=$OPTARG
            ;;
        d)
            root_dir=$OPTARG
            ;;
        e)
            continue_on_fail=true
            ;;
        c)
            compiler=$OPTARG
            ;;
        O)
            optimization_level=$OPTARG
            ;;
        a)  
            compiler_args=$OPTARG
            ;;
        ?)
            echo "Usage: unit_testing [-f file_name] [-d root_dir] [-e] [-c compiler] [-Olevel] [-a compiler_args]"
            echo ""
            echo "-f        A single file to test, if present no other files are tested."
            echo ""
            echo "-d        The root directory to search for test files and folders"
            echo "          under. If not specified the current directory is used."
            echo ""
            echo "-e        Flag for specifying continue on fail. If present a test"
            echo "          file will continue even if one of the test functions fail."
            echo ""
            echo "-c        The compiler to use, default is gcc."
            echo ""
            echo "-O        The optimization level that is sent to the compiler. As an"
            echo "          extra convenience. If '-Oall' is given the tests are run for"
            echo "          all levels of optimization. By default test are run without"
            echo "          any optimization."
            echo ""
            echo "-a        Compiler arguments, for example any linking that is needed,"
            echo "          address sanitizing or anything else."
            exit
            ;;
    esac
done

[ -z "$root_dir" ] && root_dir="*"
[ -z "$compiler" ] && compiler=gcc
[ -z "$compiler_args" ] && compiler_args="-Wall -Wpedantic"
[ -n "$continue_on_fail" ] && compiler_args="$compiler_args -DCUT_CONTINUE_ON_FAIL"

[ -z "$optimization_level" ] && optimization_level="0"
[ "$optimization_level" = all ] && run_all_optimizations=true


run_tests_under_directories () {
    for d in $1
    do
        echo "Running unit tests under: $(tput bold)$d$(tput sgr0)"
        for f in $(find $d -type f -name '*.c')
        do
            print_file_under_directory $d $f
            run_test $f
        done
        echo ""
    done
}

run_scattered_tests () {
    echo "Running scattered unit tests:"
    for f in $test_files
    do
        file_is_under_directories $f $test_directories

        if [ -z "$file_exists" ] ; then
            print_file $f
            run_test $f
        fi
    done
}


run_test () {
    $compiler -o compiled_test.out $1 "-O$optimization_level" $compiler_args
    if [ $? -eq 0 ] ; then
        ./compiled_test.out
        rm ./compiled_test.out
    fi
}

file_is_under_directories () {
    file_exists=""
    for d in $2
    do
        temp=$(find $d -wholename $1)
        if [ -n "$temp" ] ; then
            file_exists=true
            break
        fi
    done
}

print_file_under_directory () {
    local file_name=$(echo "$2" | sed -e "s:^$1/::")
    print_file $file_name
}

print_file () {
    echo "$(tput bold)$1$(tput sgr0)"
}
echo "$test_file"

if [ -n "$test_file" ] ; then
    if [ -z "$run_all_optimizations" ] ; then
        run_test $test_file
    else
        for o in "0" "1" "2" "3"
        do
            echo "---------------------------- $(tput bold)Running test with -O$o$(tput sgr0) ----------------------------"
            optimization_level=$o
            run_test $test_file
        done
    fi
    exit
fi

any_scattered=""
for f in $test_files
do
    file_is_under_directories $f $test_directories
    if [ -z "$file_exists" ] ; then
        any_scattered=true
        break
    fi
done

echo "-------------------------------------------------------------------------------"
echo "|                              Running unittests                              |"
echo "-------------------------------------------------------------------------------"

test_directories=$(find $root_dir -type d -name 'tests*')
test_files=$(find $root_dir -type f -name 'test*.c')

if [ -z "$run_all_optimizations" ] ; then
    run_tests_under_directories $test_directories

    if [ -n "$any_scattered" ] ; then
        run_scattered_tests $test_files
    fi
else
    for o in "0" "1" "2" "3"
    do
        echo "---------------------------- $(tput bold)Running tests with -O$o$(tput sgr0) ---------------------------"
        optimization_level=$o
        run_tests_under_directories $test_directories

        if [ -n "$any_scattered" ] ; then
            run_scattered_tests $test_files
        fi
    done
fi

