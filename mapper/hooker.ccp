#include canosi basegame relation keyceptor neoc consi.logger

#bodinc game

//////////////////////////////////////////////////////////////////////////////////////////
[body]
   BG::Starter starter([]{

      auto& bg = BG::bg();
      
      BG::marsiso().bloom_enabled_xnl = true;
      BG::marsiso().ssao_enabled_xnl = true;
      //BG::marsiso().dof_enabled_xnl = true;
      BG::enableDither(22, 2);
      
      BG::mk().flatground(0, -90);
      
      game->setup();
      game->print();
      
      BG::touchhandlers / [](const TouchInfo& tinfoil){
         return game->handleTouchEvent(tinfoil);
      };
      
      BG::drawer2D / []{
         game->draw2D();
      };
      
      BG::drawer3D / []{
         game->draw3D();
      };
      
      BG::updaters / []{
         game->update();
      };
      
      BG::postparticlers / []{
         game->drawPostParticles();
      };
      
      BG::manualparticles();
      
      BG::postwaver([]{
         game->drawPostWave();
      });
      
      BG::marstapper([](const TouchInfo& tinfoil, const Point2D& marspos){
         game->handleMarsTap(tinfoil);
      });
      
      /*
      _keyceptor[0](rk_a)("blah") / []{
         // do something with some keys
         return true;
      };
      */
      
      
      \\:: "Setup finished at least right?"
      
      Roen::neoc("system") / []{
             
         // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         Roen::neocruler();
         
         Roen::neoc("hello") / []{
            CALLconslog(hello, uist() / 196_xcoel / "hello was called");
         };
         
         Roen::neoc("sprites") / []{
            
            // okay, let's see eh?
            
         };
         
         // do something with some neocs
         BG::marsiso().enableNeocMenu();
         
         // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
         
      };
      
      
   });
//////////////////////////////////////////////////////////////////////////////////////////
