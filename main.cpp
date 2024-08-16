#include <iostream>
#include <stdio.h>
#include <utility>
#include <bits/stdc++.h> 

#define SIZE 9

using namespace std;

class OrderedFile {
  private:
    typedef struct Data {
      bool is_null;
      int value;

      Data(): is_null(true) {}
    } Data;

    typedef struct List {
      int size = SIZE;
      Data *values;

      List(): values(new Data[SIZE]) {}
    } List;

    List list;

    int get_range() { return log2(this->list.size); }

    int get_n(int pos, int range) {
      int n = 0;
      for(int i = pos; i < (pos + range); i++) 
        n += (!list.values[i].is_null);
      return n;
    }

    double get_density(int pos, int range) { return ((double) get_n(pos, range)/range); }

    pair<double, double> get_density_bound(int h, int d) {
      pair<double, double> bound;
      bound.first = 0.5 - (0.25 * (d/h));
      bound.second = 0.75 + (0.25 * (d/h));
      return bound;
    }

    int binary_search(int x) {
      int low = 0, high = this->list.size - 1, mid = 0;
      while(low <= high) {
        mid = low + (high - low) / 2; 

        while(this->list.values[mid].is_null && mid <= high) mid++;

        if(mid > high) {
          mid = low + (high - low) / 2; 
          while(this->list.values[mid].is_null && mid >= low) mid--;
        }

        if(mid < low) return low;
        if(this->list.values[mid].value < x) low = mid + 1;
        else high = mid - 1;
      }
      return low;
    }

    void shift_left(int pos){
      Data value = this->list.values[pos];
      while(!this->list.values[--pos].is_null) {
        Data aux = this->list.values[pos];
        this->list.values[pos] = value;
        value = aux;
      }
      this->list.values[pos] = value;
    }

    void shift_right(int pos) {
      Data value = this->list.values[pos];
      while(!this->list.values[++pos].is_null) {
        Data aux = this->list.values[pos];
        this->list.values[pos] = value;
        value = aux;
      }
      this->list.values[pos] = value;
    }

    void table_doubling() {
      int size = this->list.size * 2, j = 0;
      Data *items = new Data[size];
      
      for(int i = 0; i < this->list.size; i++) {
        if(!this->list.values[i].is_null) {
          items[j] = this->list.values[i];
          items[j].is_null = false;
          j++;
        }
      }

      delete[] this->list.values;
      this->list.size = size;
      this->list.values = items;
    }

    void redistribute(int pos, int range) {}

    void rebalance(int pos, int range) {}
    
    void scan(int pos, int range) {
      int pos_range = pos % range;
      int pos_init_range =  pos - pos_range;
      int height = log2(this->list.size);

      double density = get_density(pos_init_range, range);
      pair<double, double> bound = get_density_bound(height, height);

      if(density < bound.first || density >= bound.second)
        return rebalance(pos, pos_init_range); 

        // [0 // 1 // 2 3 4 5 10]
        //  0  1 2 3 4 5 6 7  8
    }

  public:
    void insert(int x) {
      Data item;
      item.value = x;
      item.is_null = false;

      if(this->get_n(0, this->list.size) == 0) {
        this->list.values[0] = item;
        return;
      }

      int pos = binary_search(x);
      if(this->list.values[pos].is_null) 
        this->list.values[pos] = item;
      else {
        if(this->list.values[pos].value > x) 
          this->shift_right(pos);
        else {
          pos = pos < (this->list.size - 1) ? pos + 1 : pos;
          if(!this->list.values[pos].is_null)
            this->shift_left(pos);
        }
        this->list.values[pos] = item;
      }

      for(int i = 0; i < this->list.size; i++) {
        if(this->list.values[i].is_null) 
          cout << "//" << ' ';
        else
          cout << this->list.values[i].value << ' ';
      }
      cout << endl;

      scan(pos, this->get_range());
    }
    void remove(){}
};

int main() {
  OrderedFile file;

  file.insert(1);
  file.insert(2);
  file.insert(3);
  file.insert(10);
  file.insert(8);
  file.insert(19);
  file.insert(33);
  file.insert(23);
  file.insert(22);

  return 0;
}