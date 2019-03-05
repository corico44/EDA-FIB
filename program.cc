#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME MoonsitaC9


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
   const int MAX = 10000000;
   typedef vector< vector<bool> > Visitado;
   typedef vector<int> VE;
   typedef vector< int> Arc;
   typedef vector < VE > Graph;
   typedef vector< vector <Arc> > Ciudades;
   Ciudades city;
   vector<VE> dist_wat;
   vector<VE> dist_city;
   vector<VE> dist_road;
   vector<VE> dist_stat;
   vector<VE> dist_enem;
   set<Pos> next_car;
   vector<Pos> pos_city;
   Graph enemies;

   //INICIALIZACIÓN DEL JUEGO

   void inicializacion() {
       calculo_dist_wat();
       calculo_dist_city();
       calculo_dist_road();
       calculo_dist_stat();
       city = Ciudades (60, vector<Arc>(60, vector<int>(nb_cities(), MAX)));
       pos_city = vector<Pos> (nb_cities());
       Visitado visit(60, vector<bool>(60, false));
       int i_city = 0;
       for (int i = 0; i < 60; ++i) {
           for (int j = 0; j < 60; ++j){
               Pos ini = {i,j};
               if (cell(ini).type == City) {
                   if (not visit[i][j]) {
                       visitar_ciudad(visit, ini, i_city);
                       pos_city[i_city] = ini;
                       ++i_city;
                   }
               }
           }
       }
   }
   Dir fuel_full(int id){
     enemies = Graph(60, vector<int> (60, MAX));
     Pos e = coche_hasta_warrior(unit(id).pos.i, unit(id).pos.j);
     coche_a_enemigo(e.i, e.j);
     int min = MAX;
     int direc = 8;
     Pos act;
     for(int x = 0; x < 8; ++x) {
         act = unit(id).pos + Dir(x);
         if (pos_ok(act) and min > enemies[act.i][act.j] and (cell(act).id == -1 or unit(cell(act).id).player != me()) and next_car.find(act) == next_car.end()) {
             direc = x;
             min = enemies[act.i][act.j];
         }

   }
    return Dir(direc);
 }

   Pos coche_hasta_warrior(int i, int j) {
       Visitado visit(60, vector<bool>(60, false));
       queue<Pos> Q;
       visit[i][j] = true;
       Pos act;
       Pos adjcell;
       Q.push(Pos(i,j));
       while(not Q.empty()) {
           Pos act = Q.front();
           Q.pop();
           for(int x = 0; x < 8; ++x) {
               adjcell = act + Dir(x);
               if (pos_ok(adjcell) and not visit[adjcell.i][adjcell.j]) {
                   visit[adjcell.i][adjcell.j] = true;
                   if (cell(adjcell).type == Road or siguiente_hay_road(adjcell)) {
                       if (cell(adjcell).id != -1 and unit(cell(adjcell).id).player != me() and unit(cell(adjcell).id).type != Car) return adjcell;
                       Q.push(adjcell);
                   }
               }
           }
       }
       return Pos(i,j);
   }

   bool siguiente_hay_road(Pos ini) {
       for (int x = 0; x < 8; ++x) {
           Pos adjcell = ini + Dir(x);
           if (pos_ok(adjcell) and cell(adjcell).type == Road) return true;
       }
       return false;
   }

   void coche_a_enemigo(int i, int j) {
       enemies[i][j] = 0;
       Visitado visit(60, vector<bool>(60, false));
       queue<Pos> Q;
       visit[i][j] = true;
       Q.push(Pos(i,j));
       Pos act;
       Pos adjcell;
       while(not Q.empty()) {
           act = Q.front();
           Q.pop();
           for(int x = 0; x < 8; ++x) {
               adjcell = act + Dir(x);
               if(pos_ok (adjcell) and not visit[adjcell.i][adjcell.j]) {
                   visit[adjcell.i][adjcell.j] = true;
                   if (cell(adjcell).type == Road) {
                       if (enemies[adjcell.i][adjcell.j] -1 >= enemies[act.i][act.j]) enemies[adjcell.i][adjcell.j] = enemies[act.i][act.j] + 1;
                       Q.push(adjcell);
                   }
               }
           }
       }
   }

   //CALCULO DISTANCIAS A DIFERENTES TIPOS

   void calculo_dist_wat(){
        dist_wat = vector<VE> (60, vector<int>(60));
        Visitado wat(60, vector<bool>(60, false));
        queue<Pos> where_wat;
        //Marcamos donde está el agua

        for (int i = 0; i < 60; ++i){
            for (int j = 0; j < 60; ++j){
                Pos posi = {i,j};
                if(cell(posi).type == Water){
                    where_wat.push(posi);
                    dist_wat[i][j] = 0;
                    wat[i][j] = true;
                  }
                  else dist_wat[i][j] = -1;
              }
        }
        Pos act;
        Pos adjcell;
        while (not where_wat.empty()) {
            act = where_wat.front();
            where_wat.pop();
            for (int x = 0; x < 8; ++x) {
                adjcell = act + Dir(x);
                if (pos_ok(adjcell) and not wat[adjcell.i][adjcell.j] and dist_wat[adjcell.i][adjcell.j] == -1) {
                  dist_wat[adjcell.i][adjcell.j] = dist_wat[act.i][act.j] + 1;
                  where_wat.push(adjcell);
                }
              }
          }
      }

      void calculo_dist_city(){
        dist_city = vector<VE> (60, vector<int>(60,-1));
        Visitado cit(60, vector<bool>(60, false));
        queue<Pos> where_cit;
        //Marcamos donde están las ciudades

        for (int i = 0; i < 60; ++i){
            for (int j = 0; j < 60; ++j){
                Pos posi = {i,j};
                if(cell(posi).type == City){
                    where_cit.push(posi);
                    dist_city[i][j] = 0;
                    cit[i][j] = true;

                  }
                  else dist_city[i][j] = -1;
              }
        }
        Pos act;
        Pos adjcell;
        while (not where_cit.empty()) {
            act = where_cit.front();
            where_cit.pop();
            for (int x = 0; x < 8; ++x) {
                adjcell = act + Dir(x);
                if (pos_ok(adjcell) and not cit[adjcell.i][adjcell.j] and dist_city[adjcell.i][adjcell.j] == -1) {
                  dist_city[adjcell.i][adjcell.j] = dist_city[act.i][act.j] + 1;
                  where_cit.push(adjcell);
                }
              }
          }
        }

        void calculo_dist_road(){
          dist_road = vector<VE> (60, vector<int>(60,-1));
          Visitado road(60, vector<bool>(60, false));
          queue<Pos> where_road;
          //Marcamos donde está el agua

          for (int i = 0; i < 60; ++i){
              for (int j = 0; j < 60; ++j){
                  Pos posi = {i,j};
                  if(cell(posi).type == Road){
                      where_road.push(posi);
                      dist_road[i][j] = 0;
                      road[i][j] = true;
                    }
                    if(cell(posi).type == Wall or cell(posi).type == City or cell(posi).type == Water) dist_road[i][j] = MAX;
                    else dist_road[i][j] = -1;
                }
          }
          Pos act;
          Pos adjcell;
          while (not where_road.empty()) {
              act = where_road.front();
              where_road.pop();
              for (int x = 0; x < 8; ++x) {
                  adjcell = act + Dir(x);
                  if (pos_ok(adjcell) and not road[adjcell.i][adjcell.j] and dist_road[adjcell.i][adjcell.j] == -1) {
                    dist_road[adjcell.i][adjcell.j] = dist_road[act.i][act.j] + 1;
                    where_road.push(adjcell);
                  }
                }
            }

          }

          void calculo_dist_stat(){
            dist_stat = vector<VE> (60, vector<int>(60,-1));
            Visitado stat(60, vector<bool>(60, false));
            queue<Pos> where_stat;
            //Marcamos donde está el agua

            for (int i = 0; i < 60; ++i){
                for (int j = 0; j < 60; ++j){
                    Pos posi = {i,j};
                    if(cell(posi).type == Station){

                        where_stat.push(posi);
                        dist_stat[i][j] = 0;
                        stat[i][j] = true;
                      }
                      if(cell(posi).type == Wall or cell(posi).type == City or cell(posi).type == Water) dist_stat[i][j] = MAX;
                      else dist_stat[i][j] = -1;
                  }
            }
            Pos act;
            Pos adjcell;
            while (not where_stat.empty()) {
                act = where_stat.front();
                where_stat.pop();
                for (int x = 0; x < 8; ++x) {
                    adjcell = act + Dir(x);
                    if (pos_ok(adjcell) and not stat[adjcell.i][adjcell.j] and dist_stat[adjcell.i][adjcell.j] == -1) {
                      dist_stat[adjcell.i][adjcell.j] = dist_stat[act.i][act.j] + 1;
                      where_stat.push(adjcell);
                    }
                  }
              }

            }

      //CALCULOS PARA LA RAPIDA CONQUISTA A CIUDADES POR PARTE DE LOS GUERREROS

      void calculo_dist_conquista(Pos war, int num_city) {
          Visitado distances(60, vector<bool>(60, false));
          queue<Pos> Q;
          Pos adjcell;
          Pos act;
          city[war.i][war.j][num_city] = 0;
          distances[war.i][war.j] = true;
          Q.push(war);
          while (not Q.empty()) {
              act = Q.front();
              Q.pop();
              for (int x = 0; x < 8; ++x) {
                  adjcell = act + Dir(x);
                  if (pos_ok(adjcell)){
                      if(not distances[adjcell.i][adjcell.j]){
                          distances[adjcell.i][adjcell.j] = true;
                          if (cell(adjcell).type != Water and cell(adjcell).type != Station and cell(adjcell).type != Wall){
                              if (city[adjcell.i][adjcell.j][num_city] - 1 >= city[act.i][act.j][num_city]) city[adjcell.i][adjcell.j][num_city] = city[act.i][act.j][num_city] + 1;
                              Q.push(adjcell);
                          }
                      }
                  }
              }
          }
        }

        void visitar_ciudad(vector<vector<bool>>& vec, Pos posi, int i_city) {
              vec[posi.i][posi.j] = true;
              queue<Pos> where_cit;
              Pos act;
              Pos adjcell;
              where_cit.push(posi);

              //BFS DE LA CIUDAD
              while (not where_cit.empty()) {
                    act = where_cit.front();
                    calculo_dist_conquista(act,i_city);
                    where_cit.pop();
                    for (int x = 0; x < 8; ++x) {
                          adjcell = act + Dir(x);
                          if (pos_ok(adjcell)){
                              if(cell(adjcell).type == City and not vec[adjcell.i][adjcell.j]){
                                    vec[adjcell.i][adjcell.j] = true;
                                    where_cit.push(adjcell);
                                }
                            }
                      }
                }
        }


      Dir conquista(int id){
        Pos inicial = unit(id).pos;
        int min = MAX;
        Pos adjcell;
        Dir conq = None;
        for (int i_city = 0; i_city < nb_cities(); ++i_city) {
            if(cell(pos_city[i_city]).owner != me()) {
                for (int x = 0; x < 8; ++x) {
                    adjcell = inicial + Dir(x);
                    if(pos_ok(adjcell)){
                      if(min > city[adjcell.i][adjcell.j][i_city] and (cell(adjcell).id == -1 or (cell(adjcell).id != -1 and not next_coche(adjcell) and unit(cell(adjcell).id).player != me() and unit(cell(adjcell).id).type != Car and unit(cell(adjcell).id).food < unit(id).food and unit(cell(adjcell).id).water < unit(id).water))){
                          conq = Dir(x);
                          min = city[adjcell.i][adjcell.j][i_city];
                      }
                    }
                 }
             }
         }
        if(conq != None and next_car.find(adjcell) == next_car.end()) return conq;
        return None;
      }
        
    bool next_coche(Pos ini){
        Pos act;
            for (int x = 0; x < 8; ++x) {
                act = ini + Dir(x);
                if (pos_ok(act)){
                  if(cell(act).id != -1 and unit(cell(act).id).player != me() and unit(cell(act).id).type == Car) return true;
                }
            }
            return false;
        }


      //TACTICAS DE ATAQUE

      int warriors_en_la_ciudad(Pos ini){
            Visitado visit(60, vector<bool>(60, false));
            queue<Pos> Q;
            Pos act;
            Pos adjcell;
            visit[ini.i][ini.j] = true;
            Q.push(ini);
            int warriors_city= 1;
            while(not Q.empty()) {
                act = Q.front();
                Q.pop();
                for (int x = 0; x < 8; ++x) {
                    adjcell = act + Dir(x);
                    if (pos_ok(adjcell) and not visit[adjcell.i][adjcell.j]){
                      visit[adjcell.i][adjcell.j] = true;
                      if(cell(adjcell).type == City){
                        if (cell(adjcell).id != -1 and unit(cell(adjcell).id).player == me()) ++warriors_city;
                        Q.push(adjcell);
                      }
                    }

                }
            }
            return warriors_city;
        }

        Dir busqueda(Pos ini){
          Pos act;
            for (int x = 0; x < 8; ++x) {
                act = ini + Dir(x);
                if (pos_ok(act)){
                  if(cell(act).type == City and cell(act).id == -1 and next_car.find(act) == next_car.end()) return Dir(x);
                }
            }
            return None;
        }

        Dir contraataque(Pos ini){
            Visitado visit(60, vector<bool>(60, false));
            queue<Pos> Q;
            Pos adjcell;
            Pos act;
            visit[ini.i][ini.j] = true;
            Q.push(ini);
            while(not Q.empty()) {
                act = Q.front();
                Q.pop();
                for (int x = 0; x < 8; ++x) {
                    adjcell = act + Dir(x);
                    if (pos_ok(adjcell) and not visit[adjcell.i][adjcell.j]){
                      visit[adjcell.i][adjcell.j] = true;
                      if(cell(adjcell).type == City){
                          if(next_car.find(adjcell) == next_car.end() and cell(adjcell).id != -1 and unit(cell(adjcell).id).player != me()) return Dir(x);
                          Q.push(adjcell);
                      }
                    }
                  }
                }

            return None;
        }


    //FUNCIONES BÁSICAS PARA LOS GUERREROS

   Dir bfs_necesita_agua(int id){

       Pos inicial = unit(id).pos;
       Pos adjcell;
       int dist_min = dist_wat[inicial.i][inicial.j];
       Dir act_dir = None;
       for(int x = 0; x < 8; ++x){
               adjcell = inicial + Dir(x);
               if(pos_ok(adjcell)){
                 if(cell(adjcell).type != Station and cell(adjcell).type != Wall and cell(adjcell).type != City){
                            if((cell(adjcell).id == -1 or (unit(cell(adjcell).id).player != me() and unit(cell(adjcell).id).type != Car)) and next_car.find(adjcell) == next_car.end()){
                                if(dist_wat[adjcell.i][adjcell.j] < dist_min){
                                    dist_min = dist_wat[adjcell.i][adjcell.j];
                                      act_dir = Dir(x);
                              }
                            }
                      }

                    }
      }
      if(act_dir != None) return act_dir;
      else return None;
   }

  //FUNCIONES PARA LOS COCHES




      Dir bfs_no_fuel(int id){
        Pos ini = unit(id).pos;

          int min = MAX;
          Dir act_dir = None;
          Pos act;
          for (int x = 0; x < 8; ++x) {
                act = ini + Dir(x);
                if (pos_ok(act) and min > dist_stat[act.i][act.j] and ((cell(act).id == -1) or (cell(act).id != -1 and unit(cell(act).id).player != me() and unit(cell(act).id).type != Car)) and next_car.find(act) == next_car.end()) {
                    act_dir = Dir(x);
                    min = dist_stat[act.i][act.j];
                }
            }
            return act_dir;
      }
    Dir rounds_move(Pos ini,int id){
          Pos act;
            for (int x = 0; x < 8; ++x) {
                act = ini + Dir(x);
                if (pos_ok(act)){
                  if(cell(act).id != -1 and unit(cell(act).id).player != me() and unit(cell(act).id).type != Car and next_car.find(act) == next_car.end()) return Dir(x);
                }
            }
            return None;
        }


      

  void move_warriors(){
        if (round()% 4 != me()) return; // This line makes a lot of sense.

          VE W = warriors(me());
          int n = W.size();
          VE perm = random_permutation(n);

        for (int i = 0; i < n; ++i) {
         // id is an own warrior. For some reason (or not) we treat our warriors in random order.
            int id = W[perm[i]];
            Pos x = {unit(id).pos.i, unit(id).pos.j};
            if(unit(id).water <= dist_wat[unit(id).pos.i][unit(id).pos.j] + 1){
                      Dir pet = bfs_necesita_agua(id);
                      command(id, pet);
                      next_car.insert(unit(id).pos+pet);
            }
          else if (cell(unit(id).pos).type == City and cell(unit(id).pos).owner != me()) {
                  Dir pet = contraataque(x);
                  command(id, pet);
                  next_car.insert(unit(id).pos + pet);

              }

            else if (cell(unit(id).pos).type == City and cell(unit(id).pos).owner == me()){
                if(round() <= 110){
                    Dir pet = rounds_move(x,id);
                    command(id, pet);
                    next_car.insert(unit(id).pos + pet);
                    
                }

                if(warriors_en_la_ciudad(x) <= 2) {
                //Defensar la ciutat: ens esperem a la posicio per a si venen enemics
                Dir pet = busqueda(x);
                command(id, pet);
                next_car.insert(unit(id).pos + pet);
                }

            }


            else{
              //Calculo del camino más corto para conquistar otras ciudades
              Dir pet = conquista(id);
              command(id, pet);
              next_car.insert(unit(id).pos+pet);
            }
        }
  }

    void move_cars() {
        vector<int> C = cars(me());
        for (int id : C) {
          if(can_move(id)){
              if(unit(id).food <= dist_stat[unit(id).pos.i][unit(id).pos.j] + 5){
                Dir pet = bfs_no_fuel(id);
                command(id,pet);
                next_car.insert(unit(id).pos+pet);
              }
              else{
                Dir x = fuel_full(id);
                command(id, x);
                next_car.insert(unit(id).pos + x);

                }
              }
}
}




    /**
     * Play method, invoked once per each round.
     */
    virtual void play () {
        if(round() == 0) inicializacion();
        move_warriors();
        move_cars();
        next_car.clear();
    }

  };


  /**
   * Do not modify the following line.
   */
  RegisterPlayer(PLAYER_NAME);
