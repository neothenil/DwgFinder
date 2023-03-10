import subprocess
import time
import psutil
import pyperclip
import _ctypes
from typing import List, Union

from . import uiautomation as auto
from pywinauto.keyboard import send_keys
from .ObjectRepo import *

from .common import zwcad_window_title
from .common import find_exe


class ZwcadException(Exception):
    pass


class ZwcadStartException(ZwcadException):
    pass


class Zwcad:
    def __init__(
        self, exe_path: str, args=None, window_title=zwcad_window_title, timeout=60
    ) -> None:
        command = [exe_path]
        if args is not None:
            command.extend(args)
        self.process = subprocess.Popen(command)
        time.sleep(0.3)
        if not psutil.pid_exists(self.process.pid):
            raise ZwcadStartException("Failed to start ZWCAD.")
        self.window = auto.WindowControl(RegexName=window_title, searchDepth=1)
        try:
            self.window.Exists(timeout)
        except _ctypes.COMError as e:
            time.sleep(0.1)
            self.window.Exists(timeout)
        self.commandline = self.window.PaneControl(
            AutomationId=CommandRoot.AutomationId, searchDepth=1
        ).PaneControl(
            AutomationId=CommandRoot.CommandLine.Command.AutomationId, searchDepth=3
        )
        count = 0
        while count < 3:
            try:
                self.commandline.Exists()
                break
            except _ctypes.COMError as e:
                time.sleep(0.1)
                count += 1
        self.commandhistory = self.window.PaneControl(
            AutomationId=CommandRoot.AutomationId, searchDepth=1
        ).PaneControl(
            AutomationId=CommandRoot.CommandLine.CommandHistory.AutomationId,
            searchDepth=3,
        )

    def send_command(self, cmd: str) -> None:
        """
        Send a single line string to ZWCAD command line.
        """
        if not cmd:
            return
        # Some characters can not be sent with uiautomation's `SendKeys`.
        # But can be sent with copyclip mechanism.
        pyperclip.copy(cmd)
        self.commandline.Click()
        time.sleep(0.1)
        send_keys("^v")
        self.commandline.SendKeys("{ENTER}")

    def invoke(self, cmdList: List[str]):
        """
        Invoke a command with user provided command line input.
        """
        for line in cmdList:
            time.sleep(0.1)
            self.send_command(line)

    def add_search_path(self, path: str):
        """
        Append `path` to ZWCADSEARCHPATH.
        """
        self.send_command('(setq newPath (getenv "ZWCADSEARCHPATH"))')
        self.send_command("(setq oldPath newPath)")
        self.send_command(f'(setq newPath (strcat newPath "{path};"))')
        self.send_command('(setenv "ZWCADSEARCHPATH" newPath)')

    def terminate(self):
        self.process.terminate()

    def crashed(self) -> Union[bool, psutil.Process]:
        """
        Test whether ZWCAD has crashed.
        """
        return find_exe("ZwCrashReport.exe")

    def wait_process(self, timeout=120):
        start_time = time.time()
        while time.time() - start_time < int(timeout):
            if self.window.WindowControl(SubName=Message.Name_QTPTest).Exists(
                Exception=False
            ):
                auto.SendKeys("{ENTER}")
                return True
            try:
                ret = self.commandline.IsEnabled
            except _ctypes.COMError as e:
                time.sleep(0.5)
            else:
                status_bar = self.window.StatusBarControl(
                    AutomationId=StatusBar.AutomationId
                )
                if status_bar.PaneControl(foundIndex=1).Exists(1, Exception=False):
                    time.sleep(0.5)
                else:
                    return True
        else:
            return False
