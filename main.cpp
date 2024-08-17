#include <iostream>
#include <stdio.h>
#include <utility>
#include <bits/stdc++.h> 

#define SIZE 8

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

    int get_n(int begin, int end) {
      int n = 0;
      for(int i = begin; i <= end; i++) 
        n += (!list.values[i].is_null);
      return n;
    }

    double get_density(int begin, int end) { return ((double) get_n(begin, end)/(end - begin + 1)); }

    pair<double, double> get_density_bound(int d) {
      pair<double, double> bound;
      int h = floor(log2(2 * this->get_range() - 1));
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
        if(this->list.values[mid].value == x) return mid;
        else if(this->list.values[mid].value < x) low = mid + 1;
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

    void redistribute(int begin, int end) {
      int n = get_n(begin, end);
      Data items[n];
      int j = 0;

      for(int i = begin; i <= end; i++) {
        if(!this->list.values[i].is_null) {
          items[j] = this->list.values[i];
          this->list.values[i].is_null = true;
          j++;
        }
      }

      int range = this->get_range();
      int pos = begin, n_values = 0;
      int depth = floor(log2(2 * this->get_range() - 1));
      pair<double, double> bound = get_density_bound(depth);

      for(int i = 0; i < j; i++) {
        this->list.values[pos + n_values] = items[i];
        this->list.values[pos + n_values].is_null = false;
        n_values++;

        if(pos == (this->list.size - 2) && (range & 1)) range = 2;
        double density = ((double) (n_values + 1.0)/(range));

        if(density < bound.first || density >= bound.second) { 
          pos += range;
          n_values = 0;
        }
      }
    }
    
    void scan(int begin, int end, int depth) {
      double density = get_density(begin, end);
      pair<double, double> bound = get_density_bound(depth);
      int size = end - begin + 1;

      if(size == this->list.size && density >= bound.second) {
        table_doubling();
        return redistribute(begin, this->list.size - 1);
      }

      int range = this->get_range();
      if(density >= bound.second) { // quando nao ta no limite
        if((begin == this->list.size - 2) && (range & 1)) {
            begin -= 2 * range; 
            scan(begin, end, depth - 2);
        }
        int index = begin / range;
        if(index & 1) begin -= size; 
        else 
          end = (end + size) >= this->list.size ? this->list.size - 1 : end + size;
        scan(begin, end, depth - 1);
      } 
      redistribute(begin, end);
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
      pos = pos >= this->list.size ? this->list.size - 1 : pos;
      if(this->list.values[pos].is_null) 
        this->list.values[pos] = item;
      else {
        if(this->list.values[pos].value > x) 
          this->shift_right(pos);
        else 
          this->shift_left(pos);
        this->list.values[pos] = item;
      }
      
      int range = this->get_range();
      int i_range = pos % range;
      int begin =  pos - i_range; 
      int depth = floor(log2(2 * this->get_range() - 1));
      int end = (begin + range - 1) >= this->list.size ? this->list.size - 1 : begin + range - 1;

      double density = get_density(begin, end);
      pair<double, double> bound = get_density_bound(depth);

      if(density < bound.first || density >= bound.second) 
        scan(begin, end, depth);
    }
    void remove(){}
};

int main() {
  OrderedFile file;

  file.insert(5);
  file.insert(4);
  file.insert(3);
  file.insert(2);
  file.insert(1);
  file.insert(19);
  file.insert(33);
  file.insert(23);
  file.insert(0);

  return 0;
}