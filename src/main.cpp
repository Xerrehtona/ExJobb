#include <exception>
#include <fstream>
#include <functional>
#include <ostream>
#include <random>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#define INSERT_THRESHOLD 17
#define DIST_SIZE 13
#define SEQ_THRESHOLD 8192

#define MIN_VALUE std::numeric_limits<double>::min()
#define MAX_VALUE std::numeric_limits<double>::max()

#define sort_pointer(sort_func) int (*sort_func)(std::vector<double>&, int, int);

#define TEST_ITERATIONS 50 
const std::vector<int> test_data_sizes = {1000000, 50000000, 100000000, 500000000, 1000000000};

int insertion_sort(std::vector<double>& arr, int left, int right)
{
    for (int i = left + 1; i <= right; i++) {
        for (int j = i; j > left && arr[j] < arr[j - 1]; j--) {
            double x = arr[j - 1];
            arr[j - 1] = arr[j];
            arr[j] = x;
        }
    }
    return 0;
}

////////////////////////////////////////////

int single_partitioning(std::vector<double>& arr, int left, int right, int& i, int& j, double pivot1)
{
    double x;
    i = left+1;
    j = right;
    while (i <= j) {
        while (arr[i] < pivot1) i++;
        while (arr[j] > pivot1) j--;
        if (i > j) break;
        x = arr[i];
        arr[i] = arr[j];
        arr[j] = x;
        i++;
        j--;
    }
    arr[left] = arr[j];
    arr[j] = pivot1;
    return 0;
}

int s_single_pivot(std::vector<double>& arr, int left, int right)
{
    int len = right-left;
    double x;

    if (len < INSERT_THRESHOLD) {
        insertion_sort(arr, left, right);
        return 0;
    }
    // median-3
    int forth = len / 4;
    int m1 = left + forth;
    int m2 = m1 + forth;
    int m3 = m2 + forth;
    if (arr[m1] > arr[m3]) { x = arr[m1]; arr[m1] = arr[m3]; arr[m3] = x; }
    if (arr[m1] > arr[m2]) { x = arr[m1]; arr[m1] = arr[m2]; arr[m2] = x; }
    if (arr[m2] > arr[m3]) { x = arr[m2]; arr[m2] = arr[m3]; arr[m3] = x; }
    double pivot = arr[m2];
    arr[m2] = arr[left];
    

    int i, j;
    single_partitioning(arr,  left,  right,  i,  j,  pivot);

    s_single_pivot(arr, left, j-1);
    s_single_pivot(arr, i, right);
    
    return 0;
}

int p_single_pivot(std::vector<double>& arr, int left, int right)
{
    int len = right-left;
    double x;

    if (len < INSERT_THRESHOLD) {
        insertion_sort(arr, left, right);
        return 0;
    }
    // median-3
    int forth = len / 4;
    int m1 = left + forth;
    int m2 = m1 + forth;
    int m3 = m2 + forth;
    if (arr[m1] > arr[m3]) { x = arr[m1]; arr[m1] = arr[m3]; arr[m3] = x; }
    if (arr[m1] > arr[m2]) { x = arr[m1]; arr[m1] = arr[m2]; arr[m2] = x; }
    if (arr[m2] > arr[m3]) { x = arr[m2]; arr[m2] = arr[m3]; arr[m3] = x; }
    double pivot = arr[m2];
    arr[m2] = arr[left];
    

    int i, j;
    single_partitioning(arr,  left,  right,  i,  j,  pivot);

    if (len < SEQ_THRESHOLD) {
        s_single_pivot(arr, left, j-1);
        s_single_pivot(arr, i, right);
    } else {
        
        std::thread first(p_single_pivot, std::ref(arr), left, j-1);
        std::thread second(p_single_pivot, std::ref(arr), i, right);
        first.join();
        second.join();
    }
    
    return 0;
}

////////////////////////////////////////////

int dual_partitioning(std::vector<double>& arr, int left, int right, int& l, int& g, double pivot1, double pivot2)
{
    double x;
    l = left + 1;
    g = right - 1;
    if (pivot1 != pivot2) {
        for (int k = l; k <= g; k++) {
            x = arr[k];
            if (x < pivot1) {
                arr[k] = arr[l];
                arr[l++] = x;
            }
            else if (x > pivot2) {
                while (arr[g] > pivot2 && k < g) {
                    g--;
                }
                arr[k] = arr[g];
                arr[g--] = x;
                x = arr[k];
                if (x < pivot1) {
                    arr[k] = arr[l];
                    arr[l++] = x;
                }
            }
        }
    }
    else {
        for (int k = l; k <= g; k++) {
            x = arr[k];
            if (x == pivot1) {
                continue;
            }
            if (x < pivot1) {
                arr[k] = arr[l];
                arr[l++] = x;
            }
            else {
                while (arr[g] > pivot2 && k < g) {
                    g--;
                }
                arr[k] = arr[g];
                arr[g--] = x;
                x = arr[k];
                if (x < pivot1) {
                    arr[k] = arr[l];
                    arr[l++] = x;
                }
            }
        }
    }
    arr[left] = arr[l - 1];
    arr[l - 1] = pivot1;
    arr[right] = arr[g + 1];
    arr[g + 1] = pivot2;
    return 0;
}

int dual_pivot_select(std::vector<double>& arr, int left, int right, double& pivot1, double& pivot2)
{
    double x;
    // pivot sampling
    int sixth = (right - left) / 6;
    int m1 = left + sixth;
    int m2 = m1 + sixth;
    int m3 = m2 + sixth;
    int m4 = m3 + sixth;
    int m5 = m4 + sixth;
    //sorting network
    if (arr[m1] > arr[m4]) { x = arr[m1]; arr[m1] = arr[m4]; arr[m4] = x; }
    if (arr[m2] > arr[m5]) { x = arr[m2]; arr[m2] = arr[m5]; arr[m5] = x; }
    if (arr[m1] > arr[m3]) { x = arr[m1]; arr[m1] = arr[m3]; arr[m3] = x; }
    if (arr[m2] > arr[m4]) { x = arr[m2]; arr[m2] = arr[m4]; arr[m4] = x; }
    if (arr[m1] > arr[m2]) { x = arr[m1]; arr[m1] = arr[m2]; arr[m2] = x; }
    if (arr[m3] > arr[m5]) { x = arr[m3]; arr[m3] = arr[m5]; arr[m5] = x; }
    if (arr[m2] > arr[m3]) { x = arr[m2]; arr[m2] = arr[m3]; arr[m3] = x; }
    if (arr[m4] > arr[m5]) { x = arr[m4]; arr[m4] = arr[m5]; arr[m5] = x; }
    if (arr[m3] > arr[m4]) { x = arr[m3]; arr[m3] = arr[m4]; arr[m4] = x; }

    pivot1 = arr[m2];
    arr[m2] = arr[left];

    pivot2 = arr[m4];
    arr[m4] = arr[right];

    return 0;
}

int s_dual_pivot_sort(std::vector<double>& arr, int left, int right)
{
    int len = right-left;

    if (len < INSERT_THRESHOLD) {
        insertion_sort(arr, left, right);
        return 0;
    }
    
    double pivot1, pivot2;
    dual_pivot_select(arr, left, right, pivot1, pivot2);

    int l,g;
    dual_partitioning(arr, left, right,l,g, pivot1, pivot2);

    s_dual_pivot_sort(arr, left, l - 2);
    s_dual_pivot_sort(arr, g + 2, right);
    
    bool diffPivots = pivot1 != pivot2;
    double x;
    if (g - l > len - DIST_SIZE && diffPivots) {
        for (int k = l; k <= g; k++) {
            x = arr[k];
            if (x == pivot1) {
                arr[k] = arr[l];
                arr[l++] = x;
            }
            else if (x == pivot2) {
                arr[k] = arr[g];
                arr[g--] = x;
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[l];
                    arr[l++] = x;
                }
            }
        }
    }

    if (pivot1 != pivot2) {
        s_dual_pivot_sort(arr, l, g);
    }

    return 0;
}

int p_dual_pivot_sort(std::vector<double>& arr, int left, int right)
{
    int len = right-left;

    if (len < INSERT_THRESHOLD) {
        insertion_sort(arr, left, right);
        return 0;
    }
    
    double pivot1, pivot2;
    dual_pivot_select(arr, left, right, pivot1, pivot2);

    int l,g;
    dual_partitioning(arr, left, right,l,g, pivot1, pivot2);

    bool diffPivots = pivot1 != pivot2;
    double x;
    if (len < SEQ_THRESHOLD) {
        s_dual_pivot_sort(arr, left, l - 2);
        s_dual_pivot_sort(arr, g + 2, right);
        
        
        // equal elements 
        if (g - l > len - DIST_SIZE && diffPivots) {
            for (int k = l; k <= g; k++) {
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[l];
                    arr[l++] = x;
                }
                else if (x == pivot2) {
                    arr[k] = arr[g];
                    arr[g--] = x;
                    x = arr[k];
                    if (x == pivot1) {
                        arr[k] = arr[l];
                        arr[l++] = x;
                    }
                }
            }
        }

        if (pivot1 != pivot2) {
            s_dual_pivot_sort(arr, l, g);
        }
    } else {
        std::thread first(p_dual_pivot_sort, std::ref(arr), left, l - 2);
        std::thread second(p_dual_pivot_sort, std::ref(arr), g + 2, right);
        std::thread third;

        if (g - l > len - DIST_SIZE && diffPivots) {
            for (int k = l; k <= g; k++) {
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[l];
                    arr[l++] = x;
                }
                else if (x == pivot2) {
                    arr[k] = arr[g];
                    arr[g--] = x;
                    x = arr[k];
                    if (x == pivot1) {
                        arr[k] = arr[l];
                        arr[l++] = x;
                    }
                }
            }
        }

        if (pivot1 != pivot2) {
            third = std::thread(p_dual_pivot_sort, std::ref(arr), l, g);
        }

        first.join();
        second.join();
        if (third.joinable()) {
            third.join();
        }
    }
    

    return 0;
}

////////////////////////////////////////////

int triple_partitioning(std::vector<double>& arr, int left, int right, int& a, int& b, int& d, double pivot1, double pivot2, double pivot3)
{
    double x;
    a = left +2; 
    b = left +2;
    int c = right -1;
    d = right -1;
    while (b <= c) {
        while (arr[b] < pivot2 && b <= c) {
            if (arr[b] < pivot1) {
                x = arr[b];
                arr[b] = arr[a];
                arr[a] = x;
                a++;
            }
            b++;
        }
        while (arr[c] > pivot2 && b <= c) {
            if (arr[c] > pivot3) {
                x = arr[d];
                arr[d] = arr[c];
                arr[c] = x;
                d--;
            }
            c--;
        }
        if (b <= c) {
            if (arr[b] > pivot3) {
                if (arr[c] < pivot1) {
                    x = arr[b];
                    arr[b] = arr[a];
                    arr[a] = arr[c];
                    arr[c] = x;
                    a++;
                } else {
                    x = arr[b];
                    arr[b] = arr[c];
                    arr[c] = x;
                }
                x = arr[d];
                arr[d] = arr[c];
                arr[c] = x;
                b++;
                c--;
                d--;
            } else {
                if (arr[c] < pivot1) {
                    x = arr[b];
                    arr[b] = arr[a];
                    arr[a] = arr[c];
                    arr[c] = x;
                    a++;
                } else {
                    x = arr[b];
                    arr[b] = arr[c];
                    arr[c] = x;
                }
                b++;
                c--;
            }
        }
    }
    a--;
    b--;
    c++;
    d++;

    arr[left+1] = arr[a];
    arr[a] = arr[b];
    arr[b] = pivot2;

    a--;

    arr[left] = arr[a];
    arr[a] = pivot1;

    arr[right] = arr[d];
    arr[d] = pivot3;

    return 0;
}

int triple_pivot_select(std::vector<double>& arr, int left, int right, double& pivot1, double& pivot2, double& pivot3)
{
    double x;
    // sampling
    int eigth = (right - left) / 8;
    int m1 = left + eigth;
    int m2 = m1 + eigth;
    int m3 = m2 + eigth;
    int m4 = m3 + eigth;
    int m5 = m4 + eigth;
    int m6 = m5 + eigth;
    int m7 = m6 + eigth;
    //sorting network
    if (arr[m1] > arr[m7]) { x = arr[m1]; arr[m1] = arr[m7]; arr[m7] = x; }
    if (arr[m3] > arr[m4]) { x = arr[m3]; arr[m3] = arr[m4]; arr[m4] = x; }
    if (arr[m5] > arr[m6]) { x = arr[m5]; arr[m5] = arr[m6]; arr[m6] = x; }
    if (arr[m2] > arr[m5]) { x = arr[m2]; arr[m2] = arr[m5]; arr[m5] = x; }
    if (arr[m1] > arr[m3]) { x = arr[m1]; arr[m1] = arr[m3]; arr[m3] = x; }
    if (arr[m4] > arr[m7]) { x = arr[m4]; arr[m4] = arr[m7]; arr[m7] = x; }
    if (arr[m1] > arr[m2]) { x = arr[m1]; arr[m1] = arr[m2]; arr[m2] = x; }
    if (arr[m4] > arr[m5]) { x = arr[m4]; arr[m4] = arr[m5]; arr[m5] = x; }
    if (arr[m3] > arr[m6]) { x = arr[m3]; arr[m3] = arr[m6]; arr[m6] = x; }
    if (arr[m2] > arr[m3]) { x = arr[m2]; arr[m2] = arr[m3]; arr[m3] = x; }
    if (arr[m5] > arr[m7]) { x = arr[m5]; arr[m5] = arr[m7]; arr[m7] = x; }
    if (arr[m3] > arr[m4]) { x = arr[m3]; arr[m3] = arr[m4]; arr[m4] = x; }
    if (arr[m5] > arr[m6]) { x = arr[m5]; arr[m5] = arr[m6]; arr[m6] = x; }
    if (arr[m2] > arr[m3]) { x = arr[m2]; arr[m2] = arr[m3]; arr[m3] = x; }
    if (arr[m4] > arr[m5]) { x = arr[m4]; arr[m4] = arr[m5]; arr[m5] = x; }
    if (arr[m6] > arr[m7]) { x = arr[m6]; arr[m6] = arr[m7]; arr[m7] = x; }

    pivot1 = arr[m2];
    arr[m2] = arr[left];

    pivot2 = arr[m4];
    arr[m4] = arr[left+1];

    pivot3 = arr[m6];
    arr[m6] = arr[right];

    return 0;
}

int s_triple_pivot_sort(std::vector<double>& arr, int left, int right)
{
    int len = right-left;

    if (len < INSERT_THRESHOLD) {
        insertion_sort(arr, left, right);
        return 0;
    }
    
    double pivot1, pivot2, pivot3;
    triple_pivot_select(arr, left, right, pivot1, pivot2, pivot3);

    int l, m, g;
    triple_partitioning(arr, left, right, l, m, g, pivot1, pivot2, pivot3);

    s_triple_pivot_sort(arr, left, l-1);
    s_triple_pivot_sort(arr, g + 1, right);


    bool diffPivots1_2 = pivot1 != pivot2;
    bool diffPivots2_3 = pivot2 != pivot3;
    double x;
    int m_l = m-1;
    int m_g = m+1;
    if (m_l - l > len - DIST_SIZE && diffPivots1_2) {
        for (int k = l; k <= m_l; k++) {
            x = arr[k];
            if (x == pivot1) {
                arr[k] = arr[l];
                arr[l++] = x;
            }
            else if (x == pivot2) {
                arr[k] = arr[m_l];
                arr[m_l--] = x;
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[l];
                    arr[l++] = x;
                }
            }
        }
    }
    if (diffPivots1_2) {
        s_triple_pivot_sort(arr, l, m_l);
    }

    if (g-1 - m_g > len - DIST_SIZE && diffPivots2_3) {
        for (int k = m_g; k <= g; k++) {
            x = arr[k];
            if (x == pivot1) {
                arr[k] = arr[m_g];
                arr[m_g++] = x;
            }
            else if (x == pivot2) {
                arr[k] = arr[g];
                arr[g--] = x;
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[m_g];
                    arr[m_g++] = x;
                }
            }
        }
    }
    if (diffPivots2_3) {
        s_triple_pivot_sort(arr, m_g, g);
    }

    return 0;
}

int p_triple_pivot_sort(std::vector<double>& arr, int left, int right)
{
    int len = right-left;

    if (len < INSERT_THRESHOLD) {
        insertion_sort(arr, left, right);
        return 0;
    }
    
    
    double pivot1, pivot2, pivot3;
    triple_pivot_select(arr, left, right, pivot1, pivot2, pivot3);

    int l, m, g;
    triple_partitioning(arr, left, right, l, m, g, pivot1, pivot2, pivot3);

    bool diffPivots1_2 = pivot1 != pivot2;
    bool diffPivots2_3 = pivot2 != pivot3;
    double x;
    int m_l = m-1;
    int m_g = m+1;
    if (len < SEQ_THRESHOLD) {
        s_triple_pivot_sort(arr, left, l-1);
        s_triple_pivot_sort(arr, g + 1, right);
        
        
        if (m_l - l > len - DIST_SIZE && diffPivots1_2) {
            for (int k = l; k <= m_l; k++) {
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[l];
                    arr[l++] = x;
                }
                else if (x == pivot2) {
                    arr[k] = arr[m_l];
                    arr[m_l--] = x;
                    x = arr[k];
                    if (x == pivot1) {
                        arr[k] = arr[l];
                        arr[l++] = x;
                    }
                }
            }
        }
        if (diffPivots1_2) {
            s_triple_pivot_sort(arr, l, m_l);
        }

        if (g - m_g > len - DIST_SIZE && diffPivots2_3) {
            for (int k = m_g; k <= g; k++) {
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[m_g];
                    arr[m_g++] = x;
                }
                else if (x == pivot2) {
                    arr[k] = arr[g];
                    arr[g--] = x;
                    x = arr[k];
                    if (x == pivot1) {
                        arr[k] = arr[m_g];
                        arr[m_g++] = x;
                    }
                }
            }
        }
        if (diffPivots2_3) {
            s_triple_pivot_sort(arr, m_g, g);
        }
        
        
    } else {
        std::thread first(p_triple_pivot_sort, std::ref(arr), left, l-1);
        std::thread second(p_triple_pivot_sort, std::ref(arr), g + 1, right);
        std::thread third;
        std::thread fourth;
        
        
        if (m_l - l > len - DIST_SIZE && diffPivots1_2) {
            for (int k = l; k <= m_l; k++) {
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[l];
                    arr[l++] = x;
                }
                else if (x == pivot2) {
                    arr[k] = arr[m_l];
                    arr[m_l--] = x;
                    x = arr[k];
                    if (x == pivot1) {
                        arr[k] = arr[l];
                        arr[l++] = x;
                    }
                }
            }
        }
        if (diffPivots1_2) {
            third = std::thread(p_triple_pivot_sort, std::ref(arr), l, m_l);
        }

        if (g - m_g > len - DIST_SIZE && diffPivots2_3) {
            for (int k = m_g; k <= g; k++) {
                x = arr[k];
                if (x == pivot1) {
                    arr[k] = arr[m_g];
                    arr[m_g++] = x;
                }
                else if (x == pivot2) {
                    arr[k] = arr[g];
                    arr[g--] = x;
                    x = arr[k];
                    if (x == pivot1) {
                        arr[k] = arr[l];
                        arr[m_g++] = x;
                    }
                }
            }
        }
        if (diffPivots2_3) {
            fourth = std::thread(p_triple_pivot_sort, std::ref(arr), m_g, g);
        }
        first.join();
        second.join();
        if (third.joinable()) {
            third.join();
        }
        if (fourth.joinable()) {
            fourth.join();
        }
    }
    
    return 0;
}

////////////////////////////////////////////

bool is_sorted(std::vector<double>& arr)
{
    for(long long unsigned int i = 1; i < arr.size(); i++){
        if(arr[i-1] > arr[i]) return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    std::uniform_int_distribution<int> dist(1, 10000000);
    std::uniform_real_distribution<double> unif(MIN_VALUE, MAX_VALUE);

    std::default_random_engine re_0;
    re_0.seed(0);

    
    
    for (long long unsigned int i = 0; i < test_data_sizes.size(); i++) {
        std::ofstream file(std::string("output_") + std::to_string(test_data_sizes[i]) + ".csv");
        try {
            file << "Size, Pivot count, Time [ms]\n";
            for (int j = 0; j < TEST_ITERATIONS; j++) {
                int seed = dist(re_0);
                for (int k = 0; k < 3; k++) {
                    std::vector<double> t_a;
                    std::default_random_engine re;
                    re.seed(seed);
                    for (int l = 0; l < test_data_sizes[i]; l++) {
                        t_a.push_back(unif(re));
                    }
                    sort_pointer(s_p);
                    switch (k) {
                        case 0:
                        s_p = &p_single_pivot;
                        break;
                        case 1:
                        s_p = &p_dual_pivot_sort;
                        break;
                        case 2:
                        s_p = &p_triple_pivot_sort;
                        break;
                        default:
                        throw std::runtime_error("index out of bounds");
                    }
                    
                    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                    s_p(t_a, 0, t_a.size()-1);
                    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                    if (is_sorted(t_a)) {
                        file << test_data_sizes[i] << ", " << k + 1 << ", " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "\n";
                    } else {
                        throw std::runtime_error("Vector not sorted");
                    }

                }
            }
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        file.close();
    }

    return 0;
};

