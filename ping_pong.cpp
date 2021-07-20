/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  simple_fifo.cpp -- Simple SystemC 2.0 producer/consumer example.

                     From "An Introduction to System Level Modeling in
                     SystemC 2.0". By Stuart Swan, Cadence Design Systems.
                     Available at www.accellera.org

  Original Author: Stuart Swan, Cadence Design Systems, 2001-06-18

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include <systemc.h>

class write_if : virtual public sc_interface
{
   public:
     virtual void write(char) = 0;
};

class read_if : virtual public sc_interface
{
   public:
     virtual void read(char &) = 0;
};

class ping_pong : public sc_channel, public write_if, public read_if
{
   public:
     ping_pong(sc_module_name name) : sc_channel(name),
	empty{true, true}, w_cur{0, 0}, r_cur{0, 0}, wi(0), ri(0)
     {
     
     }

     void write(char c) {
       if (! empty[wi])
         wait(buf_empty);

       buf[wi][w_cur[wi]] = c;
       cout << sc_time_stamp() << ": write buf " << wi << ": " << c << endl;
       w_cur[wi] = (w_cur[wi] + 1) % max;

       if (w_cur[wi] == 0) {
         empty[wi] = false;
	       wi = wi ^ 1;
	       buf_full.notify();
       }
     }

     void read(char &c) {
       if (empty[ri])
         wait(buf_full);

       c = buf[ri][r_cur[ri]];
       cout << sc_time_stamp() << ": read  buf " << ri << ": " << c << endl;
       r_cur[ri] = (r_cur[ri] + 1) % max;

       if (r_cur[ri] == 0) {
         empty[ri] = true;
	       ri = ri ^ 1;
         buf_empty.notify();
       }
     }

   private:
     enum e { max = 10 };

     sc_event buf_empty, buf_full;
     char buf[2][max]; // 2 buf
     bool empty[2]; // is buf empty ?
     int w_cur[2]; // current write location
     int r_cur[2]; // current read location
     int wi, ri; // which buffer is writing/reading
};

class producer : public sc_module
{
   public:
     sc_port<write_if> out;

     SC_HAS_PROCESS(producer);

     producer(sc_module_name name) : sc_module(name)
     {
       SC_THREAD(main);
     }

     void main()
     {
       const char *str =
         "Visit www.accellera.org and see what SystemC can do for you today!\n";

       while (*str) {
         out->write(*str);
	 //cout << sc_time_stamp() << ": write " << *str << endl;
	 str++;
	 wait(1, SC_SEC);
       }
     }
};

class consumer : public sc_module
{
   public:
     sc_port<read_if> in;

     SC_HAS_PROCESS(consumer);

     consumer(sc_module_name name) : sc_module(name)
     {
       SC_THREAD(main);
     }

     void main()
     {
       char c;

       while (true) {
         in->read(c);
	 //cout << sc_time_stamp() << ": read " << c << endl;
	 wait(1, SC_SEC);
       }
     }
};

class top : public sc_module
{
   public:
     ping_pong *ping_pong_inst;
     producer *prod_inst;
     consumer *cons_inst;

     top(sc_module_name name) : sc_module(name)
     {
       ping_pong_inst = new ping_pong("Pingpong1");

       prod_inst = new producer("Producer1");
       prod_inst->out(*ping_pong_inst);

       cons_inst = new consumer("Consumer1");
       cons_inst->in(*ping_pong_inst);
     }
};

int sc_main (int, char *[]) {
   top top1("Top1");
   sc_start();
   return 0;
}
