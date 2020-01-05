////////////////////////////////////////////////////////
//
//    Ripped from .propedit command
//    Modified look'n'feel by Kri
//    All credits to Dougall (dougalloogle@yahoo.com), fine work
//
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Constants
//  - ! Warning ! code will probably be unstable
//  if these consts are varied.
const MAX_X             := 600;
const MAX_Y             := 300;
const TEXT_LINES        := 10;
const MAX_PAGES         := 6;
//
const GUMP_BACKGROUND   := BKGND_DARK_STONE; // (scroll)

////////////////////////////////////////////////////////
// Global Variables
var gump_width     :=1;
var show_in_hex    :=1;
var finished       :=0;

////////////////////////////////////////////////////////

function Prop_Editor( who, i )
  var thing;
  if( i == 0 )
    thing := acc;
  else
    thing := acc.GetCharacter(i);
  endif
  if (!thing)
    sendsysmessage(who,CANCELED);
    return;
  else
    ReserveItem(thing);
    while (SendCpropGump(who,thing))
        sleepms(100);
    endwhile
  endif
  if (!thing)
    sendsysmessage(who,"Item destroyed!");
  else
    ReleaseItem(thing);
  endif
endfunction

////////////////////////////////////////////////////////

function SendCpropGump(who,thing)
  var propnames:=array;
  var edit_cprop:=array;
  var delete_cprop:=array;
  var apply_buttons:=array;
  var add_buttons:=array;
  var this_page,this_text,this_prop,first_num,last_num,set_propname,button,num;
  var add_cprop,confirm_delete,confirm_add,hextoggle,widthtoggle;
  var numpages := 0;

  propnames:=GetObjPropertyNames(thing);
  num := len(propnames);
  while ((num > 0) or (numpages=0))
    num := num - TEXT_LINES;
    numpages := numpages + 1;
  endwhile
  if (numpages>MAX_PAGES)
    numpages:=MAX_PAGES;
  endif
  this_text:=thing.name;
  if (!this_text)
    this_text:=thing.desc;
  endif

  ComposeMainPage(this_text,hextoggle,widthtoggle);
  for (this_page := 1; this_page <= numpages; this_page:= this_page + 1)
    ComposeCpropPage(this_page, thing , propnames , edit_cprop, delete_cprop, apply_buttons, numpages);
  endfor
  ComposeAddPropPage(numpages+1,set_propname,add_buttons,confirm_add);
  ComposeConfirmDeletePage(numpages+2,confirm_delete);

  var retkey := GFSendGump(who);

  if (!thing)
    return 0;
  endif

  if (retkey[hextoggle])
    show_in_hex:=(!show_in_hex);
    return 1;
  elseif (retkey[widthtoggle]);
    gump_width:=gump_width+1;
    if (!GetGumpWidth())
      gump_width:=1;
    endif
    return 1;
  endif

  if (retkey[confirm_add])          // add cprop
    var propname:=GetGumpText(retkey[set_propname]);
    if (!propname)
      return 1;
    elseif (GetObjProperty(thing,propname))
      sendsysmessage(who,"That Propname already exists!");
      return 1;
    endif
    if (retkey[add_buttons[1]])
      SetObjProperty(thing,propname,Cint(0));
    elseif (retkey[add_buttons[2]])
      SetObjProperty(thing,propname,Cdbl(0));
    else
      SetObjProperty(thing,propname,"");
    endif
    return 1;
  endif

  if (retkey[confirm_delete])          // delete marked cprops
    for (num :=1 ; num <= delete_cprop.size(); num := num +1)
      if (retkey[delete_cprop[num]])
        EraseObjProperty(thing,propnames[num]);
      endif
    endfor
    return 1;
  endif

  foreach button in apply_buttons
    if (retkey[button])          // main apply button pressed
      for (num := 1; num <= edit_cprop.size(); num := num + 1)
        if (edit_cprop[num])
          this_prop:=GetObjProperty(thing,propnames[num]);
          this_text:=GetGumpText(retkey[edit_cprop[num]]);
          SetObjProperty(thing,propnames[num],SetPropType(this_text,typeof(this_prop)));
        endif
      endfor
      return 1;
    endif
  endforeach

  return 0;
endfunction

////////////////////////////////////////////////////////

function ComposeMainPage(thing_name,byref hextoggle,byref widthtoggle)
  GFInitGump(0, 0);
  GFPage(0);
  GFResizePic(0, 0,  GUMP_BACKGROUND , GetGumpWidth(), MAX_Y);
  if (!show_in_hex)
    GFTextLine(70, MAX_Y-30, COLOR_RED, "Show Hex");
  else
    GFTextLine(70, MAX_Y-30 , COLOR_RED, "Show Int");
  endif
  var button:=GFButtonID(50,MAX_Y-27,0x8b0,0x8b0,1);
  hextoggle:=button.keyid;
  button:=GFButtonID(GetGumpWidth()-40,10,0x01f,0x01f,1);
  widthtoggle:=button.keyid;
  GFTextLine(40, 9, 0 , "Prop Editor:");
  GFTextLine(43,11,50,"Prop Editor:");
  GFTextCrop(140, 10, (GetGumpWidth()-200), 25, COLOR_BLUE , (thing_name));
endfunction

////////////////////////////////////////////////////////

function ComposeCpropPage(page,item,byref props,byref edit_cprop,byref delete_cprop,byref apply_buttons, numpages)
  var button,propnum,this_prop;
  var y := 60;
  var ysize := 20;
  var first_num := TEXT_LINES * (page - 1) + 1;
  var last_num := TEXT_LINES * page;
  GFPage(page);
  if (page < numpages)
    GFButtonPage(GetGumpWidth() - 60, MAX_Y - 35, 0xfc, 0xfd, page + 1);
  endif
  if (page > 1)
     GFButtonPage(GetGumpWidth() - 60, 10, 0xfa , 0x0fb, page - 1);
  endif

  if (numpages>1)
    GFGumpPic(GetGumpWidth()-108,MAX_Y-37,0xa54);
    GFGumpPic(GetGumpWidth()-105,MAX_Y-33,(page+0x8b0));
  endif

  GFButtonPage(340,38,0x8b0,0x8b0,numpages+1);
  GFTextLine(360, 35, 34, "Add New Cprop");
  GFButtonPage(40,38,0x8b0,0x8b0,numpages+2);
  GFTextLine(60, 35, 34, "Del Marked Cprops");

  GFButtonID(380,(MAX_Y-33),0x0817,0x0818,1);
  button:=GFButtonID(280,MAX_Y-33,0x0ef,0x0f0,1);
  apply_buttons.append(button.keyid);
  for (propnum := first_num; propnum <= last_num; propnum := propnum + 1)
    if (props[propnum])
      this_prop:=GetObjProperty(item,props[propnum]);
      GFTextCrop(50,y,130,y+ysize,COLOR_BLUE,props[propnum]);
      GFTextCrop(200,y,60,y+ysize,COLOR_BLUE,TypeOf(this_prop));
      if (this_prop.errortext)
        GFTextCrop(280,y,(GetGumpWidth()-310),20,COLOR_RED,this_prop.errortext);
        edit_cprop.append(0);
      elseif ( (TypeOf(this_prop)="Integer") or (TypeOf(this_prop)="Double") or (TypeOf(this_prop)="String") )
        GFGumpPic(260,y+3,0x03a);
        button:=(GFTextEntry(280,y,(GetGumpWidth()-310),ysize,COLOR_BLUE,ToggleHex(this_prop)));
        edit_cprop.append(button.keyid);
      else
        GFTextCrop(280,y,(GetGumpWidth()-310),20,COLOR_BLUE,Cstr(this_prop));
        edit_cprop.append(0);
      endif
      button:=GFCheckBox(30,y,0xd2,0xd3,0);
      delete_cprop.append(button.keyid);
      y := y + ysize;
    endif
  endfor
endfunction

////////////////////////////////////////////////////////

function ComposeAddPropPage(page,byref set_propname,byref add_buttons,byref confirm_add)
  var button;
  GFPage(page);
  GFResizePic(160, 25, GUMP_BACKGROUND, 340, (MAX_Y-50));
  GFTextLine(280, 34, 1, "Add Cprop");
  GFTextLine(283, 35,34,"Add Cprop");
  GFTextLine(200,70,0,"Enter new propname....");
  GFTextLine(200,120,0,"Choose proptype....");
  GFTextLine(260,144,0,"Integer");
  GFTextLine(260,174,0,"Double");
  GFTextLine(260,204,0,"String");
  GFGumpPic(190,90,0x477);

  button:=GFTextEntry(200,92,200,20,0,"");
  set_propname:=button.keyid;
  button:=GFRadioButton(220,140,0x868,0x86a,1);
  add_buttons[1]:=(button.keyid);
  button:=GFRadioButton(220,170,0x868,0x86a,0);
  add_buttons[2]:=(button.keyid);
  button:=GFRadioButton(220,200,0x868,0x86a,0);
  add_buttons[3]:=(button.keyid);

  GFButtonPage(380,(MAX_Y-58),0x0817,0x0818,1);
  button:=GFButtonID(230,(MAX_Y-58),0x0ef,0x0f0,1);
  confirm_add:=(button.keyid);          // apply button for add page
endfunction

////////////////////////////////////////////////////////

function ComposeConfirmDeletePage(page,byref confirm_delete)
  GFPage(page);
  GFResizePic(160, 25, GUMP_BACKGROUND, 340, (MAX_Y-50));
  GFTextLine(260,34,1,"Confirm Delete");
  GFTextLine(263,36,34,"Confirm Delete");
  GFTextLine(240,110,0,"Delete Checked Cprops ?");
  GFButtonPage(380,(MAX_Y-58),0x0817,0x0818,1);
  var button:=GFButtonID(230,(MAX_Y-58),0x0ef,0x0f0,1);
  confirm_delete:=(button.keyid);
endfunction

////////////////////////////////////////////////////////

function GetGumpText(text)
  return text[Find(text,": ", 1)+2, len(text)];
endfunction

////////////////////////////////////////////////////////

function GetGumpWidth()
  case (gump_width)
    1:  return MAX_X;
    2:  return MAX_X+150;
    3:  return MAX_X+300;
  endcase
  return 0;
endfunction

////////////////////////////////////////////////////////

function SetPropType(text,type)
  case (type)
    "Integer" : return Cint(text);
    "Double"  : return Cdbl(text);
  endcase
  return text;
endfunction

////////////////////////////////////////////////////////

function ToggleHex(check)
  if ((TypeOf(check)="Integer") and (show_in_hex))
    return Hex(check);
  else
    return Cstr(check);
  endif
endfunction

////////////////////////////////////////////////////////