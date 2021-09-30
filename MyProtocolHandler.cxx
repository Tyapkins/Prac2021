/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include "ListenerHelper.h"
#include "MyProtocolHandler.h"
#include <iostream>
#include <typeinfo>
#include <random>

#include <com/sun/star/awt/MessageBoxButtons.hpp>
#include <com/sun/star/awt/Toolkit.hpp>
#include <com/sun/star/awt/XMessageBoxFactory.hpp>
#include <com/sun/star/frame/ControlCommand.hpp>
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/system/SystemShellExecute.hpp>
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#include <com/sun/star/system/XSystemShellExecute.hpp>
#include <cppuhelper/supportsservice.hxx>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/xml/sax/Writer.hpp>
#include <com/sun/star/document/DocumentProperties.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>
#include <com/sun/star/frame/XToolbarController.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleText.hpp>
#include <com/sun/star/accessibility/XAccessibleValue.hpp>
#include <com/sun/star/accessibility/XAccessibleEditableText.hpp>
#include <com/sun/star/awt/XSpinField.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/ui/XUIConfigurationManager.hpp>
#include <com/sun/star/frame/XLoaderFactory.hpp>
#include <com/sun/star/text/XTextTable.hpp>
#include <com/sun/star/table/XCell.hpp>

using namespace com::sun::star::awt;
using namespace com::sun::star::frame;
using namespace com::sun::star::system;
using namespace com::sun::star::uno;

using com::sun::star::beans::NamedValue;
using com::sun::star::beans::PropertyValue;
using com::sun::star::sheet::XSpreadsheetView;
using com::sun::star::text::XTextViewCursorSupplier;
using com::sun::star::util::URL;

ListenerHelper aListenerHelper;

void BaseDispatch::ShowMessageBox( const Reference< XFrame >& rFrame, const ::rtl::OUString& aTitle, const ::rtl::OUString& aMsgText )
{
    if ( !mxToolkit.is() )
        mxToolkit = Toolkit::create(mxContext);
    Reference< XMessageBoxFactory > xMsgBoxFactory( mxToolkit, UNO_QUERY );
    if ( rFrame.is() && xMsgBoxFactory.is() )
    {
        Reference< XMessageBox > xMsgBox = xMsgBoxFactory->createMessageBox(
            Reference< XWindowPeer >( rFrame->getContainerWindow(), UNO_QUERY ),
            com::sun::star::awt::MessageBoxType_INFOBOX,
            MessageBoxButtons::BUTTONS_OK,
            aTitle,
            aMsgText );

        if ( xMsgBox.is() )
            xMsgBox->execute();
    }
}

void BaseDispatch::SendCommand( const com::sun::star::util::URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    Reference < XDispatch > xDispatch =
            aListenerHelper.GetDispatch( mxFrame, aURL.Path );

    FeatureStateEvent aEvent;

    aEvent.FeatureURL = aURL;
    aEvent.Source     = xDispatch;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;

    aEvent.State <<= aCtrlCmd;
    aListenerHelper.Notify( mxFrame, aEvent.FeatureURL.Path, aEvent );
}

void BaseDispatch::SendCommandTo( const Reference< XStatusListener >& xControl, const URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    FeatureStateEvent aEvent;

    aEvent.FeatureURL = aURL;
    aEvent.Source     = (::com::sun::star::frame::XDispatch*) this;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;

    aEvent.State <<= aCtrlCmd;
    xControl->statusChanged( aEvent );
}

void SAL_CALL MyProtocolHandler::initialize( const Sequence< Any >& aArguments )
{
    std::random_device rd;
    std::mt19937 gen(rd());
    Reference < XFrame > xFrame;
    if ( aArguments.getLength() )
    {
        // the first Argument is always the Frame, as a ProtocolHandler needs to have access
        // to the context in which it is invoked.
        aArguments[0] >>= xFrame;
        mxFrame = xFrame;
    }
}

Reference< XDispatch > SAL_CALL MyProtocolHandler::queryDispatch(   const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags )
{
    Reference < XDispatch > xRet;
    if ( !mxFrame.is() )
        return 0;

    Reference < XController > xCtrl = mxFrame->getController();
    if ( xCtrl.is() && aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        Reference < XTextViewCursorSupplier > xCursor( xCtrl, UNO_QUERY );
        Reference < XSpreadsheetView > xView( xCtrl, UNO_QUERY );
        if ( !xCursor.is() && !xView.is() )
            // without an appropriate corresponding document the handler doesn't function
            return xRet;

        if ( aURL.Path == "EditfieldCmd" ||
             aURL.Path == "SpinfieldCmd" ||
             aURL.Path == "GenerateButton" ||
             aURL.Path == "StatButton" ||
             aURL.Path == "HighlightButton" ||
             aURL.Path == "DropdownboxCmd" )
        {
            xRet = aListenerHelper.GetDispatch( mxFrame, aURL.Path );
            if ( !xRet.is() )
            {
                xRet = xCursor.is() ? (BaseDispatch*) new WriterDispatch( mxContext, mxFrame ) :
                    (BaseDispatch*) new CalcDispatch( mxContext, mxFrame );
                aListenerHelper.AddDispatch( xRet, mxFrame, aURL.Path );
            }
        }
    }

    return xRet;
}

Sequence < Reference< XDispatch > > SAL_CALL MyProtocolHandler::queryDispatches( const Sequence < DispatchDescriptor >& seqDescripts )
{
    sal_Int32 nCount = seqDescripts.getLength();
    Sequence < Reference < XDispatch > > lDispatcher( nCount );

    for( sal_Int32 i=0; i<nCount; ++i )
        lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL, seqDescripts[i].FrameName, seqDescripts[i].SearchFlags );

    return lDispatcher;
}

::rtl::OUString MyProtocolHandler_getImplementationName ()
{
    return ::rtl::OUString( MYPROTOCOLHANDLER_IMPLEMENTATIONNAME );
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler_getSupportedServiceNames(  )
{
    Sequence < ::rtl::OUString > aRet(1);
    aRet[0] = ::rtl::OUString( MYPROTOCOLHANDLER_SERVICENAME );
    return aRet;
}

#undef SERVICE_NAME

Reference< XInterface > SAL_CALL MyProtocolHandler_createInstance( const Reference< XComponentContext > & rSMgr)
{
    return (cppu::OWeakObject*) new MyProtocolHandler( rSMgr );
}

// XServiceInfo
::rtl::OUString SAL_CALL MyProtocolHandler::getImplementationName(  )
{
    return MyProtocolHandler_getImplementationName();
}

sal_Bool SAL_CALL MyProtocolHandler::supportsService( const ::rtl::OUString& rServiceName )
{
    return cppu::supportsService(this, rServiceName);
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler::getSupportedServiceNames(  )
{
    return MyProtocolHandler_getSupportedServiceNames();
}

rtl::OUString makeAlphabet(sal_Unicode first_let, sal_Unicode last_let)
{
    rtl::OUString alphabet;
    for (auto letter = first_let; letter <= last_let; letter++)
    {
        rtl::OUString next_let(letter);
        alphabet += next_let;
    }
    return alphabet;
}

rtl::OUString generateWord(rtl::OUString alphabet, int max_len)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> len_gen(1, max_len);
    std::uniform_int_distribution<> letter_gen(0, alphabet.getLength()-1);
    rtl::OUString word;
    auto word_len = len_gen(gen);
    for (int i = 0; i < word_len; i++)
    {
        rtl::OUString next_letter(alphabet[letter_gen(gen)]);
        word += next_letter;
    }
    return word;

}

rtl::OUString generateWords(rtl::OUString alphabet, int word_num, int max_len)
{
    rtl::OUString words;
    rtl::OUString space(" ");
    for (int i = 0; i < word_num; i++)
        words += generateWord(alphabet, max_len) + space;
    return words.trim();
}

long wordContains(rtl::OUString word, rtl::OUString alphabet)
{
    auto check_maxlen = word.getLength();
    for (int i = 0; i < check_maxlen; i++)
    {
        rtl::OUString letter(word[i]);
        if (alphabet.indexOf(letter) != -1)
            return i;
    }
    return -1;
}

rtl::OUString getsubstr(rtl::OUString word, rtl::OUString alphabet, long start_pos)
{
    rtl::OUString new_word = rtl::OUString(word[start_pos]);
    for (int i = start_pos + 1; i < word.getLength(); i++) {
        rtl::OUString new_letter(word[i]);
        if (wordContains(alphabet, new_letter) != -1)
            new_word += new_letter;
        else
            break;
    }
    return new_word;
}

long extract_pos_num(rtl::OUString word)
{
    auto numbers = makeAlphabet(L'0', L'9');
    auto bHasNumber = wordContains(word, numbers);
    long num = -1;
    if ( bHasNumber != -1) {
        auto sub = getsubstr(word, numbers, bHasNumber);
        num = sub.toInt64();
    }
    return num;
}

sal_Unicode toLowerLat(sal_Unicode sym)
{
    if ((L'A' <= sym) && (sym <= L'Z'))
        return sym + (L'a'-L'A');
    else
        return sym;
}

sal_Unicode toLowerCyr(sal_Unicode sym)
{
    if (sym == L'Ё')
        return L'ё';
    else if (((L'А') <= sym) && (sym <= L'Я'))
        return sym + (L'а'-L'А');
    else
        return sym;
}

bool isLat(sal_Unicode sym)
{
    return ((L'a' <= toLowerLat(sym)) && (toLowerLat(sym) <= L'z'));
}

bool isCyr(sal_Unicode sym)
{
    return (((L'а' <= toLowerCyr(sym)) && (toLowerCyr(sym) <= L'я')) || (toLowerCyr(sym) == L'ё'));
}

std::map<sal_Unicode, int> get_stat(rtl::OUString text)
{
    std::map<sal_Unicode, int> stats;
    auto textlen = text.getLength();
    for (int i = 0; i < textlen; i++)
    {
        auto sym = text[i];
        if (isLat(sym) || isCyr(sym)) {
            sym = toLowerCyr(toLowerLat(sym));
            auto sym_pos = stats.find(sym);
            if (sym_pos != stats.end()) {
                (sym_pos->second)++;
            } else {
                stats[sym] = 1;
            }
        }
    }
    return stats;
}

rtl::OUString get_text_no_table(Reference <com::sun::star::text::XTextCursor> cursor)
{
    rtl::OUString new_word, full_text;
    cursor->gotoStart(false);
    while(cursor->goRight(1,true))
    {
        new_word = cursor->getString();
        full_text += rtl::OUString(new_word[new_word.getLength()-1]);
    }
    return full_text;
}

Reference <com::sun::star::text::XTextTable>
        create_table(Reference <com::sun::star::text::XTextDocument> mxDoc,
                  int columns, int rows)
{

    Reference<com::sun::star::lang::XMultiServiceFactory> oDocMSF (mxDoc, UNO_QUERY);
    Reference <com::sun::star::text::XTextTable> table (oDocMSF->createInstance("com.sun.star.text.TextTable"),UNO_QUERY);

    auto text = mxDoc -> getText();

    table -> initialize(columns, rows);

    auto xTextRange = text->getEnd();
    Reference <com::sun::star::text::XTextContent> xTextContent (table,UNO_QUERY);
    text ->insertTextContent(xTextRange, xTextContent, 0);

    return table;
}

void fill_cell_string(Reference <com::sun::star::text::XTextTable> table, rtl::OUString name, rtl::OUString value)
{
    auto cell = table->getCellByName(name);
    Reference<com::sun::star::text::XText> cellText(cell, UNO_QUERY);
    auto cursor = cellText -> createTextCursor();
    cursor -> setString(value);
};

void fill_cell_num(Reference <com::sun::star::text::XTextTable> table, rtl::OUString name, long value)
{
    auto cell = table->getCellByName(name);
    cell -> setValue(value);
};

void SAL_CALL BaseDispatch::dispatch( const URL& aURL, const Sequence < PropertyValue >& lArgs )
{
    /* It's necessary to hold this object alive, till this method finishes.
       May the outside dispatch cache (implemented by the menu/toolbar!)
       forget this instance during de-/activation of frames (focus!).

        E.g. An open db beamer in combination with the My-Dialog
        can force such strange situation :-(
     */
    mxFrame -> getComponentWindow();
    Reference< XInterface > xSelfHold(static_cast< XDispatch* >(this), UNO_QUERY);

    const auto lat_alphabet = makeAlphabet(L'A', L'Z') + makeAlphabet(L'a', L'z');
    const auto cyr_alphabet = makeAlphabet(L'А', L'Е') + makeAlphabet(L'Ё', L'Ё')
                              + makeAlphabet(L'Ж', L'е') + makeAlphabet(L'ё', L'ё') + makeAlphabet(L'ж', L'я');

    if ( aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( aURL.Path == "GenerateButton" )
        {
            Reference <XComponentLoader> rComponentLoader(mxFrame, UNO_QUERY);
            Reference <com::sun::star::beans::XPropertySet> checkFrame(mxFrame, UNO_QUERY);

            auto words_len = extract_pos_num(maxLen);

            if ((words_len > 0) && (wordNum > 0)) {

                auto newWin = rComponentLoader->loadComponentFromURL("private:factory/swriter", "_blank", 0, lArgs);
                Reference <com::sun::star::text::XTextDocument> doc_text(newWin, UNO_QUERY);
                auto newText = doc_text -> getText();

                const auto cur_alphabet = (layout == "Latin") ? lat_alphabet : (layout == "Cyrillic") ?
                        cyr_alphabet : lat_alphabet+cyr_alphabet;
                newText->setString(generateWords(cur_alphabet, wordNum, words_len));
            }
            else
                ShowMessageBox(mxFrame, "Error!", "Please, fill all the fields correctly!");
        }
        else if (aURL.Path == "StatButton")
        {

            auto mxData = (mxFrame -> getController()) -> getModel();
            Reference <com::sun::star::text::XTextDocument> mxDoc(mxData, UNO_QUERY);
            auto cursor = (mxDoc->getText()) -> createTextCursor();

            auto stats = get_stat(get_text_no_table(cursor));
            auto table = create_table(mxDoc, stats.size()+1, 2);
            auto cells = table->getCellNames();

            auto i = cells.begin();

            fill_cell_string(table, *(i++), "Letter");
            fill_cell_string(table, *(i++), "Value");
            
            auto val = stats.begin();

            bool contains_bad = (stats.find(L'ё') != stats.end());
            int bad_num = 0;

            for (; i != cells.end(); i++)
            {
                if (contains_bad && (val->first > L'е'))
                {
                    bad_num = extract_pos_num(*(i++));
                    contains_bad = false;
                    i++;
                }
                if ((*i)[0] == L'A')
                    fill_cell_string(table, *i, rtl::OUString(val->first));
                else {
                    fill_cell_num(table, *i, val->second);
                    val++;
                }
            }

            if (bad_num != 0)
            {
                fill_cell_string(table, rtl::OUString(L'A') + rtl::OUString::valueOf(bad_num), rtl::OUString(L'ё'));
                fill_cell_num(table, rtl::OUString(L'B') + rtl::OUString::valueOf(bad_num), stats[L'ё']);
            }
        }
        else if (aURL.Path == "HighlightButton")
        {
            auto mxData = (mxFrame -> getController()) -> getModel();
            Reference <com::sun::star::text::XTextDocument> mxDoc(mxData, UNO_QUERY);

            auto cursor = (mxDoc->getText()) -> createTextCursor();
            Reference <com::sun::star::beans::XPropertySet> graph(cursor, UNO_QUERY);

            rtl::OUString word;
            sal_Unicode sym;

            cursor->gotoStart(false);

            while (cursor->goRight(1, true))
            {
                word = cursor->getString();
                if (word.getLength() > 0)
                {
                    sym = word[word.getLength()-1];
                    if (!(isLat(sym) || isCyr(sym))) {
                        cursor->goLeft(1,true);
                        if ((wordContains(word, lat_alphabet) != -1) && (wordContains(word, cyr_alphabet) != -1))
                            graph->setPropertyValue("CharColor", Any(0xCC1D00));
                        cursor->goRight(1, false);
                    }
                }
            }
            word = cursor->getString();
            if ((wordContains(word, lat_alphabet) != -1) && (wordContains(word, cyr_alphabet) != -1))
                graph->setPropertyValue("CharColor", Any(0xCC1D00));
        }
        else if ( aURL.Path == "DropdownboxCmd" )
        {
            for ( sal_Int32 i = 0; i < lArgs.getLength(); i++ )
            {
                if ( lArgs[i].Name == "Text" )
                {
                    lArgs[i].Value >>= layout;
                    break;
                }
            }
        }
    }
}

void SAL_CALL BaseDispatch::addStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL )
{
    if ( aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( aURL.Path == "EditfieldCmd" )
        {
            ::com::sun::star::frame::FeatureStateEvent aEvent;
            aEvent.FeatureURL = aURL;
            aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
            aEvent.IsEnabled = sal_True;
            aEvent.Requery = sal_False;
            aEvent.State = Any();
            xControl->statusChanged( aEvent );
        }
        else if ( aURL.Path == "SpinfieldCmd" )
        {
            Sequence< NamedValue > aArgs( 5 );

            aArgs[0].Name = "Value";
            aArgs[0].Value <<= wordNum;
            aArgs[1].Name = "UpperLimit";
            aArgs[1].Value <<= INTMAX_MAX-1;
            aArgs[2].Name = "LowerLimit";
            aArgs[2].Value <<= 1;
            aArgs[3].Name = "Step";
            aArgs[3].Value <<= 1;
            aArgs[4].Name = "OutputFormat";
            aArgs[4].Value <<= rtl::OUString("%d word(s)");

            SendCommandTo( xControl, aURL, rtl::OUString( "SetValues" ), aArgs, sal_True );
        }
        else if ( aURL.Path == "DropdownboxCmd" )
        {
            // A dropdown box is normally used for a group of commands
            // where the user can select one of a defined set.
            Sequence< NamedValue > aArgs( 1 );

            // send command to set context menu content
            Sequence< rtl::OUString > aList( 3 );
            aList[0] = "Latin";
            aList[1] = "Cyrillic";
            aList[2] = "Mixed";

            aArgs[0].Name = "List";
            aArgs[0].Value <<= aList;
            Sequence <NamedValue> saveArgs(1);
            saveArgs[0].Name = "Entry";
            saveArgs[0].Value <<= (layout == "Latin") ? 0 : (layout == "Cyrillic") ? 1 : 2;

            SendCommandTo( xControl, aURL, rtl::OUString( "SetList" ), aArgs, sal_True );
        }
        aListenerHelper.AddListener( mxFrame, xControl, aURL.Path );
    }
}

void SAL_CALL BaseDispatch::removeStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL )
{
    aListenerHelper.RemoveListener( mxFrame, xControl, aURL.Path );
}

void SAL_CALL BaseDispatch::controlEvent( const ControlEvent& Event )
{
    static int event_num = 0;
    if ( Event.aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( Event.aURL.Path == "EditfieldCmd" )
        {
            // We get notifications whenever the text inside the combobox has been changed.
            // We store the new text into a member.
            if ( Event.Event == "TextChanged" )
            {
                rtl::OUString aNewText;
                sal_Bool      bHasText( sal_False );
                for ( sal_Int32 i = 0; i < Event.aInformation.getLength(); i++ )
                {
                    if ( Event.aInformation[i].Name == "Text" )
                    {
                        bHasText = Event.aInformation[i].Value >>= aNewText;
                        break;
                    }
                }
                if ( bHasText )
                    maxLen = aNewText;
            }
        }
        else if (Event.aURL.Path == "SpinfieldCmd")
        {
            if ( Event.Event == "TextChanged" )
            {
                rtl::OUString aNewText;
                for ( sal_Int32 i = 0; i < Event.aInformation.getLength(); i++ )
                {
                    if ( Event.aInformation[i].Name == "Text" )
                    {
                        Event.aInformation[i].Value >>= aNewText;
                        break;
                    }
                }
                wordNum = extract_pos_num(aNewText);
            }
        }
    }
}

BaseDispatch::BaseDispatch( const Reference< XComponentContext > &rxContext,
                            const Reference< XFrame >& xFrame,
                            const ::rtl::OUString& rServiceName )
        : mxContext( rxContext )
        , mxFrame( xFrame )
        , msDocService( rServiceName )
{
}

BaseDispatch::~BaseDispatch()
{
    mxFrame.clear();
    mxContext.clear();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
