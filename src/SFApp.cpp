#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w, 88.0f);
  player->SetPosition(player_pos);

 for(int j=0; j<5; j++) {
  auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  auto pos  = Point2((canvas_w/5)*j, 100);
  coin->SetPosition(pos);
  coins.push_back(coin);
}
/*
 for(int k=0; k<5; k++) {
  auto wall = make_shared<SFAsset>(SFASSET_WALLS, sf_window);
  auto pos  = Point2((canvas_w/5)*k, 200);
  wall->SetPosition(pos);
  walls.push_back(wall);
}
*/

  for(int i=0; i<4; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((canvas_w/4) * i, 400.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }
  
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;

  case SFEVENT_PLAYER_UP:
    player->GoNorth();
    break;
  case SFEVENT_PLAYER_DOWN:
    player->GoSouth();
    break;

  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // if this is an update event, then handle it in SFApp,
    // otherwise punt it to the SFEventDispacher.
    SFEvent sfevent((const SDL_Event) event);
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  //Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }


//Detect collisions between player and wall
  for(auto w : walls) {
    if(player->CollidesWith(w)){
      player->HandleCollision();
      w->HandleCollision();
      player->GoEast();
    }
}

  // Update enemy positions
  for(auto a : aliens) {
    // do something here
  }

  // Detect collisions between player and aliens
    for(auto a : aliens) {
      if(player->CollidesWith(a)) {
        player->HandleCollision();
        a->HandleCollision();
        cout <<"GAME OVER"<< endl;
      }
    }

  //Detect collisions with coins
  for(auto c: coins) {
    
      if(player->CollidesWith(c)) {
        player->HandleCollision();
        c->HandleCollision();
      }
    }
  

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);

  // remove coins once collected
  for(auto c : coins) {
    if(c->IsAlive()) {
      tmp.push_back(c);
    }
  }
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(tmp);
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  player->OnRender();


  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    if(c->IsAlive()) {c->OnRender();}
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
