/******************************************************
 Copyright (c) netsecsp 2012-2032, All rights reserved.
 
 Developer: Shengqian Yang, from China, E-mail: netsecsp@hotmail.com, last updated 01/15/2024
 http://asynframe.sf.net
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above
 copyright notice, this list of conditions and the
 following disclaimer.
 
 * Redistributions in binary form must reproduce the
 above copyright notice, this list of conditions
 and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************/
package com.demo;

import javax.swing.*;
import com.frame.api.*;

public class Testgui {
    public static void main(String[] args) {
        
        CInstanceManager.add("ActionEvent", new CInstanceManager.IAsynMessageEvents() {
            @Override
            public int onMessage(int message, long lparam1, long lparam2, CUnknown object) {
                System.out.println("message=" + message + ", lparam1=" + lparam1 + ", lparam2=" + lparam2 + ", object=" + object + " in java");
                if (message == 6) {//通知事件
                    if (lparam1 == 0) { //系统类型
                        switch((int)lparam2) {
                            case 0: //shutdown
                                 new Thread() {
                                     @Override
                                     public void run() {
                                         System.exit(0);
                                     }
                                 }.start();
                                 break;
                            case 1: //电源休眠
                                 break;
                            case 2: //电源恢复
                                 break;
                        }
                    }
                }
                return 0;
            }
        });

        CInstanceManager.setMainType(1);

        JFrame frame = new JFrame("Hello World Example");
        JLabel label = new JLabel("Hello World");

        frame.add(label);
        frame.setSize(300, 200);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}