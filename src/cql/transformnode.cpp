#include "node.h"
// for TransformNode::transform_in_place(Transform*t) see the file transform_members.cpp

void TransformNode::expand(){
  uassert(transformedFilters.empty()&&filter);
  for(auto transform : transforms){
    MFilter* transformednode=dynamic_cast<MFilter*>(filter->transform(transform));
    uassert(transformednode,"null transform or wrong type in expand()");
    if(!transformednode->hasEmptySquareMaskDescendant())
      transformedFilters.push_back(transformednode);
  }
  for(auto transformedfilter : transformedFilters)
    transformedfilter->expand();
  filter=NULL;
}


vnode TransformNode::children(){
  vnode v;
  if(expanded())
    v.insert(v.end(),transformedFilters.begin(),transformedFilters.end());
  else
    v.push_back(filter);
  return v;
}
  
TransformNode::TransformNode(vector<Transform*>ts,MFilter*f,Range*r){
  uassert(f);
  for(auto t:ts) uassert(t);
  transforms=ts;
  filter=f;
  range=r;
}

void TransformNode::print(){
  int ntransforms=transforms.size();
  int nfilters=transformedFilters.size();
  printf("<%s ntransforms: %d nfilters: %d",thisclass(),ntransforms,nfilters);
  if(range) {
    printf("range: ");
    range->print();
  }
  for(int i=0;i<ntransforms;++i){
    auto t=transforms[i];
    printf("\n");indent();tab();
    printf("Transform %d of %d: ",i,ntransforms);
    t->print();
    unindent();
  }
  if(filter){
    uassert (!nfilters);
    printf("\n");indent();tab();
    printf("Filter: ");
    filter->print();
    unindent();
  }
  else{
    uassert (nfilters);
    for(int i=0;i<nfilters;++i){
      auto tfilter=transformedFilters[i];
      printf("\n");indent();tab();
      printf("TransformedFilter %d of %d: ",i,nfilters);
      tfilter->print();
      unindent();
    }
  }
  printf(" %s>",thisclass());
}

void TransformNode::deepify(){
  uassert(!expanded());
  filter=filter->clone();
}
    
bool TransformNode::expanded(){
  uassert(!filter&&transformedFilters.size() ||
	  filter&&transformedFilters.empty());
  return filter==NULL;
}

TransformNode* TransformNode::clone(){
  uassert(!expanded(),"cannot clone expanded transformnode");
  TransformNode*ret=new TransformNode(*this);
  ret->deepify();
  cloneverify(this,ret);
  return ret;
}

bool TransformNode::match_position(Game*game){
  uassert(expanded(),"tn not expanded");
  count=0;
  for(auto f:transformedFilters){
    if(f->match_position(game))++count;
    if(count&& !range)return true;
  }
  if(!range)return false;
  return range->valid(count);
}

bool TransformNode::match_count(Game*game,NumValue*value){
  uassert(range,"Attempt to sort or count transform that lacks a range. Only transforms with ranges can be sorted or counted");
  if(match_position(game)){
    *value=(NumValue)count;
    return true;
  }
  return false;
}

bool Node::hasEmptySquareMaskDescendant(){
  vector<Node*>ds=descendants();
  for(auto d:ds){
    uassert(d);
    if(d->hasEmptySquareMask()) return true;
  }
  return false;
}
