////////////////////////////////////////////////////////////
#include "game.cl.hpp"
#include "hooker.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
   BG::Starter starter([]{
      auto& bg = BG::bg();
      BG::marsiso().bloom_enabled_xnl = true;
      BG::marsiso().ssao_enabled_xnl = true;
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
      croust>>240<<'['>>238<<":">>235<<59>>240<<']'>>250<<' '<<"Setup finished at least right?" << ozendl;
      Roen::neoc("system") / []{
         Roen::neocruler();
         Roen::neoc("hello") / []{
            CALLconslog(hello, uist() / 196_xcoel / "hello was called");
         };
         Roen::neoc("sprites") / []{
         };
         BG::marsiso().enableNeocMenu();
      };
   });
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
