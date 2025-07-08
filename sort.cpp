#include <vector>
#include <string>
#include <iostream>
using namespace std;

class SortFactory {
public:
    template<typename T, typename Comp = less<T>>
    static void insertion_sort(vector<T>& elements, Comp comp = Comp()) {
        for (int i = 1;i < elements.size();i++) {
            T key = elements[i];
            int j = i - 1;
            while (j >= 0 && !comp(elements[j], key)) {
                elements[j + 1] = elements[j];
                j--;
            }
            elements[j + 1] = key;
        }
    }

    template<typename T, typename Comp = less<T>>
    static void selection_sort(vector<T>& elements, Comp comp = Comp()) {
        for (int i = 0;i < elements.size();i++) {
            T target_key = elements[i];
            int target_index = i;
            for (int j = i + 1;j < elements.size();j++) {
                if (comp(elements[j], target_key)) {
                    target_key = elements[j];
                    target_index = j;
                }
            }
            T temp = elements[i];
            elements[i] = target_key;
            elements[target_index] = temp;
        }
    }

    template<typename T, typename Comp = less<T>>
    static void bubble_sort(vector<T>& elements, Comp comp = Comp()) {
        for (int i = 0;i < elements.size();i++) {
            for (int j = elements.size() - 1;j > i;j--) {
                if (comp(elements[j], elements[j - 1])) {
                    T temp = elements[j];
                    elements[j] = elements[j - 1];
                    elements[j - 1] = temp;
                }
            }
        }
    }

    template<typename T, typename Comp = less<T>>
    static void merge_sort(vector<T>& elements, Comp comp = Comp()) {
        merge_sort(elements, 0, elements.size() - 1, comp);
    }

    template<typename T, typename Comp = less<T>>
    static void quick_sort(vector<T>& elements, Comp comp = Comp()) {
        quick_sort(elements, 0, elements.size() - 1, comp);
    }

private:
    template<typename T, typename Comp = less<T>>
    static void merge_sort(vector<T>& elements, int left, int right, Comp comp = Comp()) {
        if (left >= right) {
            return;
        }
        int mid = (left + right) / 2;
        merge_sort(elements, left, mid, comp);
        merge_sort(elements, mid + 1, right);
        merge(elements, left, mid, right, comp);
    }

    template<typename T, typename Comp = less<T>>
    static void merge(vector<T>& elements, int left, int mid, int right, Comp comp = Comp()) {
        vector<T> tempElements;
        int index_1 = left;
        int index_2 = mid + 1;
        while (index_1 <= mid && index_2 <= right) {
            if (comp(elements[index_1], elements[index_2])) {
                tempElements.push_back(elements[index_1]);
                index_1++;
            } else {
                tempElements.push_back(elements[index_2]);
                index_2++;
            }
        }
        while (index_1 <= mid) {
            tempElements.push_back(elements[index_1]);
            index_1++;
        }
        while (index_2 <= right) {
            tempElements.push_back(elements[index_2]);
            index_2++;
        }
        for (int i = left;i <= right;i++) {
            elements[i] = tempElements[i - left];
        }
    }

    template<typename T, typename Comp = less<T>>
    static void quick_sort(vector<T>& elements, int left, int right, Comp comp = Comp()) {
        if (left >= right) {
            return;
        }
        int i = partition(elements, left, right, comp);
        quick_sort(elements, left, i - 1, comp);
        quick_sort(elements, i + 1, right, comp);
    }

    template<typename T, typename Comp = less<T>>
    static int partition(vector<T>& elements, int left, int right, Comp comp = Comp()) {
        T key = elements[right];
        int i = left;
        for (int j = left;j < right;j++) {
            if (comp(elements[j], key)) {
                T temp = elements[i];
                elements[i] = elements[j];
                elements[j] = temp;
                i++;
            }
        }
        elements[right] = elements[i];
        elements[i] = key;
        return i;
    }
};

int main() {
    vector<int> nums = {1, 2, 4, 8, 7, 6, 5, 3};
    vector<string> strings = {"d", "dad", "hiehogaw", "ajahgoi", "ajidshf", "zzzz"};
    // SortFactory::insertion_sort(nums);
    // SortFactory::insertion_sort(strings);
    // SortFactory::selection_sort(nums);
    // SortFactory::selection_sort(strings);
    // SortFactory::bubble_sort(nums);
    // SortFactory::bubble_sort(strings);
    // SortFactory::merge_sort(nums);
    // SortFactory::merge_sort(strings);
    SortFactory::quick_sort(nums);
    SortFactory::quick_sort(strings);
    for (int num : nums) {
        cout << num << " ";
    }
    cout << endl;
    for (string str : strings) {
        cout << str << " ";
    }
}