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

    int get_n(int pos, int range) {
      int n = 0;
      for(int i = pos; i < (pos + range) && i < this->list.size; i++) 
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

    void redistribute(int pos, int range) {
      int n = get_n(pos, range);
      Data items[n];
      int j = 0;

      for(int i = pos; i < (pos + range) && i < this->list.size; i++) {
        if(!this->list.values[i].is_null) {
          items[j] = this->list.values[i];
          this->list.values[i].is_null = true;
          j++;
        }
      }

      int i_pos = pos;
      double step = ceil(((double) range)/n);
      for(int i = 0; i < j; i++) {
        int in = i_pos;
        this->list.values[i_pos] = items[i];
        this->list.values[i_pos].is_null = false;
        i_pos = (i_pos + step) >= (pos + range) ? i_pos + 1 : i_pos + step;
        if(i_pos >= this->list.size) shift_right(i_pos);
      }
    }
    
    void scan(int pos, int range, int height, int depth) {
      if(range >= this->list.size) {
        table_doubling();
        return redistribute(pos, this->list.size);
      }
      
      int pos_range = pos % range;
      int pos_init_range =  pos - pos_range;

      double density = get_density(pos_init_range, range);
      pair<double, double> bound = get_density_bound(height, depth);

      if(density < bound.first || density >= bound.second) { // quando nao ta no limite
        if(pos + range >= this->list.size) { pos = pos - range; }
        scan(pos, range + range, height, depth - 1);
      } 
      else if(range != this->get_range()) redistribute(pos, range);
      
    }

  public:
    void insert(int x) {
      cout << "INC " << x << endl;
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
        else 
          this->shift_left(pos);
        this->list.values[pos] = item;
      }
      int height = log2(this->list.size);

      for(int i = 0; i < this->list.size; i++) {
        if(this->list.values[i].is_null) 
          cout << "//" << ' ';
        else
          cout << this->list.values[i].value << ' ';
      }
      cout << endl;

      scan(pos, this->get_range(), height, height);

      for(int i = 0; i < this->list.size; i++) {
        if(this->list.values[i].is_null) 
          cout << "//" << ' ';
        else
          cout << this->list.values[i].value << ' ';
      }
      cout << endl;

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