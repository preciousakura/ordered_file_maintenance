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

      int get_range() { return log2(this->size); }

      int get_height() { return floor(log2(this->size/this->get_range())); }

      int get_n(int begin, int end) {
        int n = 0;
        for(int i = begin; i <= end; i++) 
          n += (!this->values[i].is_null);
        return n;
      }

      double get_density(int begin, int end, int n) { return ((double) n/(end - begin + 1)); }

      pair<double, double> get_density_bound(int d) {
        pair<double, double> bound;
        int h = this->get_height();
        bound.first = 0.5 - (0.25 * (d/h));
        bound.second = 0.75 + (0.25 * (d/h));
        return bound;
      }
      
      void shift_left(int pos){
        Data value = this->values[pos];
        while(!this->values[--pos].is_null) {
          Data aux = this->values[pos];
          this->values[pos] = value;
          value = aux;
        }
        this->values[pos] = value;
      }

      void shift_right(int pos) {
        Data value = this->values[pos];
        while(!this->values[++pos].is_null) {
          Data aux = this->values[pos];
          this->values[pos] = value;
          value = aux;
        }
        this->values[pos] = value;
      }

      void change_size(int size) {
        int j = 0;
        size = size < SIZE ? SIZE : size;
        Data *items = new Data[size];
        
        for(int i = 0; i < this->size; i++) {
          if(!this->values[i].is_null) {
            items[j] = this->values[i];
            items[j].is_null = false;
            j++;
          }
        }

        delete[] this->values;
        this->size = size;
        this->values = items;
      }

      void print() {
        for(int i = 0; i < size; i++) {
          if(this->values[i].is_null) cout << "//" << ' ';
          else cout << this->values[i].value << ' ';
        }
        cout << endl;
      }

      void print(int begin, int end) {
        for(int i = begin; i <= end; i++) {
          if(this->values[i].is_null) cout << "//" << ' ';
          else cout << this->values[i].value << ' ';
        }
        cout << endl;
      }
    } List;

    List list;

    int binary_search(int x) {
      int low = 0, high = this->list.size - 1, mid = 0;
      while(low <= high) {
        mid = low + (high - low) / 2; 

        while(this->list.values[mid].is_null && mid <= high) mid++;

        if(mid > high) {
          mid = low + (high - low) / 2; 
          while(this->list.values[mid].is_null && mid >= low) mid--;
        }

        if(this->list.values[mid].value == x) return mid;
        if(mid < low) return low;
        else if(this->list.values[mid].value < x) low = mid + 1;
        else high = mid - 1;
      }
      return low;
    }

    void redistribute(int begin, int end) {
      int n = this->list.get_n(begin, end);
      int blank_space = (end - begin + 1) - n;
      int steps = ceil((double) blank_space / (n - 1));

      Data items[n];
      int j = 0;

      for(int i = begin; i <= end; i++) {
        if(!this->list.values[i].is_null) {
          items[j] = this->list.values[i];
          this->list.values[i].is_null = true;
          j++;
        }
      }

      int pos = begin;
      for(int i = 0; i < j; i++) {
        this->list.values[pos] = items[i];
        if(pos + steps + 1 > end) { pos += 1; }
        else { pos += steps + 1; } 
      }
    }

  public:
    void insert(int x) {
      Data item;
      item.value = x;
      item.is_null = false;

      if(this->list.get_n(0, this->list.size - 1) == 0) {
        this->list.values[0] = item;
        return;
      }

      int pos = binary_search(x);   

      if(this->list.values[pos].is_null) {
        this->list.values[pos] = item;
        return;
      }

      int range = this->list.get_range();
      int begin =  pos - (pos % range); 
      int end = (begin + range - 1) >= this->list.size ? this->list.size - 1 : begin + range - 1;

      int end_total = end;
      int begin_total = begin;

      int size = end - begin + 1;
      int element_count = this->list.get_n(begin, end) + 1;
      int leaf_idx = begin / range;
      int range_increase = 1;
      
      double density = this->list.get_density(begin, end, element_count);
      int depth = this->list.get_height();
      pair<double, double> bound = this->list.get_density_bound(depth);
      bool out_bounds =  density > bound.second || density < bound.first;

      if(!out_bounds) {
        if(this->list.values[pos].value > x) 
          this->list.shift_right(pos);
        else 
          this->list.shift_left(pos);
        this->list.values[pos] = item;
        return;
      }

      while(out_bounds && !((end_total - begin_total + 1) == this->list.size)) {
        int leaf_size = ceil((double)size/range);
        if(size < range * leaf_size) {
          end = begin - 1;
          int b = begin - range_increase * 2 * range;
          begin = b < 0 ? 0 : b;
          depth--;
        }
        else if (leaf_idx & 1) {
          end = begin - 1;
          begin = begin - range_increase * range;
        }
        else {
          begin = end + 1;  
          end = end + range_increase * range;
          if(end > this->list.size - 1) 
            end = this->list.size - 1;
        }

        range_increase *= 2;
        leaf_idx = leaf_idx >> 1;
        begin_total =  begin < begin_total ? begin : begin_total;
        end_total =  end > end_total ? end : end_total;

        size += end - begin + 1; 
        element_count += this->list.get_n(begin, end);
        density = ((double) element_count / size); 
        depth--;

        bound = this->list.get_density_bound(depth);
        out_bounds =  density > bound.second || density < bound.first;

        if (((end_total - begin_total + 1) == this->list.size) && out_bounds) {
          this->list.change_size(this->list.size * 2);
          break;
        }
      }

      if (this->list.values[pos].is_null) 
        this->list.values[pos] = item;
      else if(this->list.values[pos].value > x) 
        this->list.shift_right(pos);
      else 
        this->list.shift_left(pos);
      this->list.values[pos] = item;

      redistribute(begin_total, end_total);
    }

    void remove(int x){
      int pos = binary_search(x);
    }
    
    void print() { this->list.print(); }
};

int main() {
  OrderedFile file;

file.insert(145);
file.print();
file.insert(108);
file.print();
file.insert(162);
file.print();
file.insert(189);
file.print();
file.insert(136);
file.print();
file.insert(151);
file.print();
file.insert(172);
file.print();
file.insert(110);
file.print();
file.insert(196);
file.print();
file.insert(130);
file.print();
file.insert(177);
file.print();
file.insert(122);
file.print();
file.insert(185);
file.print();
file.insert(133);
file.print();
file.insert(101);
file.print();
file.insert(198);
file.print();
file.insert(154);
file.print();
file.insert(123);
file.print();
file.insert(139);
file.print();
file.insert(147);
file.print();
file.insert(113);
file.print();
file.insert(103);
file.print();
file.insert(194);
file.print();
file.insert(159);
file.print();
file.insert(179);
file.print();
file.insert(165);
file.print();
file.insert(173);
file.print();
file.insert(199);
file.print();
file.insert(109);
file.print();
file.insert(141);
file.print();
file.insert(132);
file.print();
file.insert(114);
file.print();
file.insert(160);
file.print();
file.insert(111);
file.print();
file.insert(193);
file.print();
file.insert(107);
file.print();
file.insert(169);
file.print();
file.insert(155);
file.print();
file.insert(175);
file.print();
file.insert(116);
file.print();
file.insert(187);
file.print();
file.insert(125);
file.print();
file.insert(144);
file.print();
file.insert(153);
file.print();
file.insert(120);
file.print();
file.insert(140);
file.print();
file.insert(167);
file.print();
file.insert(135);
file.print();
file.insert(190);
file.print();
file.insert(128);
file.print();
file.insert(118);
file.print();
file.insert(176);
file.print();
file.insert(104);
file.print();
file.insert(102);
file.print();
file.insert(121);
file.print();
file.insert(142);
file.print();
file.insert(157);
file.print();
file.insert(126);
file.print();
file.insert(192);
file.print();
file.insert(148);
file.print();
file.insert(138);
file.print();
file.insert(112);
file.print();
file.insert(149);
file.print();
file.insert(124);
file.print();
file.insert(158);
file.print();
file.insert(106);
file.print();
file.insert(161);
file.print();
file.insert(129);
file.print();
file.insert(131);
file.print();
file.insert(156);
file.print();
file.insert(117);
file.print();
file.insert(163);
file.print();
file.insert(186);
file.print();
file.insert(152);
file.print();
file.insert(119);
file.print();
file.insert(143);
file.print();
file.insert(137);
file.print();
file.insert(166);
file.print();
file.insert(134);
file.print();
file.insert(115);
file.print();
file.insert(150);
file.print();
file.insert(200);
file.print();
file.insert(105);
file.print();
file.insert(191);
file.print();
file.insert(164);
file.print();
file.insert(146);
file.print();
file.insert(170);
file.print();
file.insert(188);
file.print();
file.insert(171);
file.print();
file.insert(197);
file.print();
file.insert(168);
file.print();
file.insert(180);
file.print();
file.insert(127);
file.print();
file.insert(184);
file.print();
file.insert(181);
file.print();
file.insert(178);
file.print();
file.insert(182);
file.print();
file.insert(174);
file.print();
file.insert(183);
file.print();
file.insert(100);
file.print();

  return 0;
}