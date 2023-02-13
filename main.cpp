#include <SDKDDKVer.h>
#include <tgbot/tgbot.h>
#include "NN.hpp"

using namespace TgBot;
using namespace std;

InlineKeyboardMarkup::Ptr keybord(initializer_list<vector<InlineKeyboardButton::Ptr>> rows) {
    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);

    for (int i = 0; i < rows.size(); i++)
        keyboard->inlineKeyboard.push_back(*(rows.begin() + i));

    return keyboard;
}

InlineKeyboardButton::Ptr button(const string& text, const string& callbackData) {
    InlineKeyboardButton::Ptr button(new InlineKeyboardButton);
    button->text = text;
    button->callbackData = callbackData;

    return button;
}

vector<InlineKeyboardButton::Ptr> row(initializer_list<InlineKeyboardButton::Ptr> buttons) {
    vector<InlineKeyboardButton::Ptr> row;

    for (int i = 0; i < buttons.size(); i++)
        row.push_back(*(buttons.begin()+i));

    return row;
}

void set_commands(Bot& bot, initializer_list<string> commands) {
    vector<BotCommand::Ptr> cmds;
    for (int i = 0; i < commands.size(); i++) {
        BotCommand::Ptr cmd(new BotCommand);
        cmd->command = *(commands.begin()+i);
        cmd->description = *(commands.begin() + i);
        cmds.push_back(cmd);
    }
    bot.getApi().setMyCommands(cmds);
}

vector<int> depi(string& text) {
    text.erase(remove(text.begin(), text.end(), ' '), text.end());
    vector<int> result;

    size_t start = 0, end = text.find(',');
    int k;
    bool flag = true;
    while (flag) {
        if (end > text.size()) {
            end = text.size();
            flag = false;
        }
        try {
            k = stoi(text.substr(start, end));
            result.push_back(k);
        }
        catch (...) {
            flag = false;
        }

        if (flag) {
            start = end + 1;
            end = text.find(',', start);
            while (end == start) {
                start++;
                end = text.find(',', start);
            }
        }
    }

    return result;
}

int main() {
    Bot bot("TOKEN");
    set_commands(bot, { "start" });

    InlineKeyboardMarkup::Ptr keyboard_1 = keybord({row({button("create", "create")})});
    InlineKeyboardMarkup::Ptr keyboard_2 = keybord({row({button("classification", "classification"), button("cancel", "cancel_create")})});
    InlineKeyboardMarkup::Ptr keyboard_3 = keybord({ row({button("cancel", "cancel_type")}) });
    InlineKeyboardMarkup::Ptr keyboard_4 = keybord({
        row({button("double", "double")}),
        row({button("cancel", "cancel_struct")})
    });
    InlineKeyboardMarkup::Ptr keyboard_5 = keybord({ 
        row({button("train", "train"), button("predict", "predict")}),
        row({button("export", "export"), button("cancel", "cancel_data")}) 
    });

    InlineKeyboardMarkup::Ptr keyboard_6 = keybord({
        row({button("import dataset", "idataset"), button("import weights", "iweights")}),
        row({button("cancel", "cancel_main")})
    });
    InlineKeyboardMarkup::Ptr keyboard_7 = keybord({
        row({button("change ALPHA", "ALPHA"), button("change NUM_EPOCHS", "NUM_EPOCHS")}),
        row({button("change BATCH_SIZE", "BATCH_SIZE"), button("cancel", "cancel_main")})
    });
    InlineKeyboardMarkup::Ptr keyboard_8 = keybord({ row({button("cancel", "cancel_main")}) });
    InlineKeyboardMarkup::Ptr keyboard_9 = keybord({
        row({button("export weights", "eweights"), button("generate files", "files")}),
        row({button("cancel", "cancel_main")})
    });

    Message::Ptr start_message;
    string type_ai;
    vector<int> structure;
    string response;
    NN<double> ai_d;

    bot.getEvents().onCommand("start", [&](TgBot::Message::Ptr message) {
        response = "Hi! Create your own neural network with me";
        start_message = bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard_1);
    });
    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "create")) {
            response = "Select the type of neural network:";
            bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_2);
        }
        else if (StringTools::startsWith(query->data, "cancel_create")) {
            response = "Hi! Create your own neural network with me";
            bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_1);
        }
    });

    bool state_type = false;
    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "classification")) {
            state_type = true;
            type_ai = "classification";
            response = "Write down the structure of the neural network (example: 3, 4, 2 - 3 layers):";
            bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_3);
        }
        else if (StringTools::startsWith(query->data, "cancel_type")) {
            response = "Select the type of neural network:";
            bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_2);
        }
    });

    bool flag_error = true;
    bot.getEvents().onAnyMessage([&](Message::Ptr message) {
        if (state_type && message->text.size() != 0) {
            bot.getApi().deleteMessage(message->chat->id, message->messageId);

            structure = depi(message->text);
            if (structure.size() < 2) {
                structure.clear();
                if (flag_error) {
                    response = "Error! Layers should be minimum 2.\nWrite down the structure of the neural network (example: 3, 4, 2 - 3 layers):";
                    bot.getApi().editMessageText(response, message->chat->id, start_message->messageId, "", "", false, keyboard_3);
                    flag_error = false;
                }
            }
            else {
                flag_error = true;
                bot.getApi().editMessageText("Select the type of data you want to work with", message->chat->id, start_message->messageId, "", "", false, keyboard_4);
                state_type = false;
            }
        }
        else if (state_type && message->text.size() == 0)
            bot.getApi().deleteMessage(message->chat->id, message->messageId);
    });

    bool flag_type = false;
    bool trained = false;
    string dataType;
    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "cancel_struct")) {
            state_type = true;
            response = "Write down the structure of the neural network (example: 3, 4, 2 - 3 layers):";
            bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_3);
        }
        else if (StringTools::startsWith(query->data, "double")) {
            dataType = "double";
            ai_d.set_config(type_ai, structure);
            flag_type = true;
        }
        if (flag_type) {
            flag_type = false;
            response = "Now your neural network has parameters:\n"\
                "\ttype: " + type_ai + "\n"\
                "\tdata type: " + dataType + "\n"\
                "\tlayers:\n";
            int k;
            for (int i = 0; i < structure.size(); i++) {
                k = structure[i];
                response += "\t\t" + to_string(i + 1) + ": " + to_string(k) + ' ' + (k != 1 ? "neurons" : "neuron") + "\n";
            }
            string t_trained = trained ? "true" : "false";
            response += "Trained: " + t_trained + '\n';

            response += "\nYou can go back and change the neural network parameters or continue setting it up";
            bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_5);
        }
    });

    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "cancel_data")) {
            flag_error = true;
            bot.getApi().editMessageText("Select the type of data you want to work with", query->message->chat->id, start_message->messageId, "", "", false, keyboard_4);
            state_type = false;
        }
        else if (StringTools::startsWith(query->data, "train")) {
            if (!trained) {
                response = "You need to load a dataset or weights for training\n"\
                    "Select an action";
                bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_6);
            }
            else {
                response = "You can adjust the hyperparameters or continue\n"\
                    "Current hypermeters:\n"\
                    "\tALPHA = " + to_string(ai_d.ALPHA) + "\n"\
                    "\tNUM_EPOCHS = " + to_string(ai_d.NUM_EPOCHS) + "\n"\
                    "\tBATCH_SIZE = " + to_string(ai_d.BATCH_SIZE) + "\n";

                bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_7);
            }
        }
        else if (StringTools::startsWith(query->data, "predict")) {
            if (!trained) {
                response = "You need to load a dataset or weights for training\n";
                bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_6);
            }
            else {
                bot.getApi().editMessageText("Enter or download data for the prediction", query->message->chat->id, start_message->messageId, "", "", false, keyboard_8);
            }
        }
        else if (StringTools::startsWith(query->data, "export")) {
            response = "You can export neural network weights or generate a C++ project with your neural network";
            bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_9);
        }
    });

    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "cancel_main")) {
            response = "Now your neural network has parameters:\n"\
                "\ttype: " + type_ai + "\n"\
                "\tdata type: " + dataType + "\n"\
                "\tlayers:\n";
            int k;
            for (int i = 0; i < structure.size(); i++) {
                k = structure[i];
                response += "\t\t" + to_string(i + 1) + ": " + to_string(k) + ' ' + (k != 1 ? "neurons" : "neuron") + "\n";
            }
            string t_trained = trained ? "true" : "false";
            response += "Trained: " + t_trained + '\n';

            response += "\nYou can go back and change the neural network parameters or continue setting it up";
            bot.getApi().editMessageText(response, query->message->chat->id, start_message->messageId, "", "", false, keyboard_5);
        }
    });

    try {
        TgLongPoll longPoll(bot);
        printf("Long poll started\n");
        while (true) {
            longPoll.start();
        }
    }
    catch (TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}
