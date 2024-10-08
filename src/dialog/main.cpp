
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for ButtonOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"  // for gauge, separator, text, vbox, operator|, Element, border
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Green, Color::Red

#include "ftxui/dom/elements.hpp"  // for Element, text, separator, window, hbox, vbox
#include "ftxui/screen/screen.hpp"  // for Screen


#include "dialog.h"

using namespace ftxui;

const ButtonOption
OneDialogPage::DefBtnsStyle = ButtonOption::Animated(Color::Blue3);

OneDialogPage::OneDialogPage(const char* name, const char* text, Options options,
        const NextOption nextOption/*={}*/,
        ButtonOption btns_style/*= DefBtnsStyle*/
) : name_(name), text_(text), btns_style(btns_style), nextOption(nextOption) {
    btns = Container::Horizontal({});
    // Options
    if (!nextOption.optGiven) { // if no nextOption is given
        this->nextOption.opt = btns_style; // uses {btns_style}
    }
    for (const auto& option: options) {
        auto option_text = option.first;
        auto onclick = [func = option.second]() { func(); };
        btns->Add(Button(option_text, onclick, btns_style)
            |flex  // this makes button stretch through the width
        );
        //|size(HEIGHT, EQUAL, 1)
        // XXX: adopting above to Button seems no use?!
    }
}

DialogPage::DialogPage(): cur(pages.end()), idx(-1) {}

DialogPage::~DialogPage(){

}

DialogPage& DialogPage::add(OneDialogPage&  n){return addImpl(this, n);}
DialogPage& DialogPage::add(OneDialogPage&& n){return addImpl(this, n);}

// addNext<T> is in header

Component DialogPage::asComponent(Element abovePart/*=filler()*/) {
    // Title
    // Text

    // Combine elements

    assert(!pages.empty());
    cur = pages.begin();
    idx = -1;
    auto res = DialogPage::ComponentWithRC::Renderer(*this);
    res->Add(cur->btns);
    res->addNextBtn();
    // we add RC to this so that lambdas below won't refer to a dead(deallocated) `this`
    res->setRender([abovePart, res]{
        auto cap_this = res->p;
        auto c = cap_this->cur;
        bool alignMsgRight = c->nextOption.textRightAlign;
        if (cap_this->hasOldBtns()){
            // update the dispatched object
            res->Add(c->btns);
            res->SetActiveChild(c->btns);
            //btns->TakeFocus(); // same as above
            cap_this->getOldBtns()->Detach();
        }
        auto msg = paragraph(std::string(c->text_)); // align left by default
        if (alignMsgRight) msg = msg | align_right;
        return vbox({
            abovePart /*empty elem to fill the space*/,
            vbox({
                //separator(), // adding this results too large room used
                vbox(vbox({
                    text(std::string(c->name_) + ":"),
                    msg  
                }) | border,
                (c->btns->Render())),
            })
        });
    });
    //auto parent = res; //std::make_shared<ComponentBase*>(std::move(btns->Parent())); // add a ref count to parent
    return res; //ComponentWithRC(res, nextPage, isNew, parent);
}


void DialogPage::mainloop(ScreenInteractive& screen) {
    auto renderer = asComponent();

    screen.Loop(renderer);

}
//DialogPage(const DialogPage& o): name_(o.name_), text_(o.text_), btns(btns), nextPage(){}

